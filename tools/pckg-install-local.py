import argparse
import logging
import json
import subprocess
import sys

import http.server
import socketserver
import threading
import time
import os


# Create logging
logging.basicConfig(level=logging.WARNING, format="%(levelname)s: %(message)s")


class LocalServer:
    """
    Local server to host the package assets
    """

    def __init__(self, pckg_dir, port=8000):
        """
        Creates a local server to host the package assets
        in the package directory

        Args:
            - pckg_dir: Path to the package directory
            - port: Port number for the local server
        """
        self.server = None
        self.port = port
        logging.info(f"Starting thread to run local server")
        self.server_thread = threading.Thread(
            target=self.__start_server, args=(pckg_dir,)
        )
        self.server_thread.daemon = True
        self.server_thread.start()

    def get_url(self):
        """
        Returns the URL of the local server
        """
        return "http://localhost:" + str(self.port)

    def __del__(self):
        """
        Deletes the local server object
        """
        self.__stop_server()

    """ Private class methods """

    def __start_server(self, pckg_dir):
        """
        Starts the local server to host the package assets
        in the package directory

        Args:
            - pckg_dir: Path to the package directory
        """
        logging.info(f'Starting local server at port "{self.port}"')

        # This class allows us to start the server in the package directory
        # without having to change the directory
        class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
            def __init__(self, *args, directory=None, **kwargs):
                self.directory = directory
                super().__init__(*args, directory=directory, **kwargs)

        handler = lambda *args, **kwargs: CustomHTTPRequestHandler(
            *args, directory=pckg_dir, **kwargs
        )

        self.server = socketserver.TCPServer(("", self.port), handler)
        print(f'Serving at port "{self.port}" in directory "{pckg_dir}"')
        self.server.serve_forever()

    def __stop_server(self):
        """
        Stops the local server
        """
        if self.server:
            logging.info(f"Stopping local server")
            self.server.shutdown()
            self.server.server_close()
            self.server_thread.join()


class PckgLocalInstaller:
    """
    Local package installer

    The class will take care of creating a new package index file
    with the local URL of the local server and install the package.
    """

    def __init__(self, pckg_dir, server_url):
        """
        Creates a new package index file with the local URL of the local server
        and installs the package

        Args:
            - pckg_dir: Path to the package directory
            - server_url: URL of the local server
        """
        self.pckg_dir = pckg_dir
        self.server_url = server_url
        self.__create_local_index()
        self.__core_uninstall()
        self.__core_install()

    """ Private class methods """

    def __create_local_index(self):
        """
        Creates a new package index file with the local URL of the local server

        The function will look for the package index file and
        the package archive in the package directory
        """
        # Check if the package index file exists in the package directory

        def find_pckg_index(pckg_dir):
            """
            Finds the package index file in the package directory

            Args:
                - pckg_dir: Path to the package directory
            """
            for file in os.listdir(pckg_dir):
                # Look for a file with ".json" extension
                if file.endswith(".json"):
                    logging.info(f'Found potential index file "{file}"')
                    return file, os.path.join(pckg_dir, file)

            logging.error(f'Package index file "{self.index_file}" not found')
            sys.exit(1)

        def find_pckg_archive_name(pckg_dir):
            """
            Finds the arduino core package archive file in the package directory

            Args:
                - pckg_dir: Path to the package directory
            """
            for file in os.listdir(pckg_dir):
                if file.endswith(".zip"):
                    logging.info(f'Found potential archive file "{file}"')
                    return file

            logging.error(f"Package archive file not found")
            sys.exit(1)

        def load_pckg_index(index_file):
            """
            Loads the package index file

            Args:
                - index_file: Path to the package index file
            """
            logging.info(f'Loading package index file "{index_file}"')
            with open(index_file, "r") as f:
                # Load the json package index
                return json.load(f)

        def set_local_url(pckg_index, url, pckg_name):
            """
            Sets the local URL in the package index file

            Args:
                - pckg_index: Package index json object
                - url: Local server URL
                - pckg_name: Package archive name
            """
            logging.info(f"Setting local URL in the package index file")

            # The URL is set in the first package and first platform
            # For local installation, we assume there is only one platform.
            # The current working version of the core.
            pckg_index["packages"][0]["platforms"][0]["url"] = (
                str(url) + "/" + str(pckg_name)
            )

        def save_local_index(pckg_index, pckg_index_name):
            """
            Saves the local package index file and return the the file including path

            The local package index file is saved with the original name
            with _local appended to it

            Args:
                - pckg_index: Package index json object
                - pckg_index_name: Package index file name

            """
            # The local package name is the original name with
            # _local appended to it
            pckg_index_name = pckg_index_name.split(".")
            local_pckg_index_name = pckg_index_name[0] + "_local." + pckg_index_name[1]
            local_pckg_index_file = os.path.join(self.pckg_dir, local_pckg_index_name)

            pckg_index = json.dumps(pckg_index, indent=4)
            logging.info(f"Saving local package index file")
            with open(local_pckg_index_file, "w") as f:
                f.write(pckg_index)

            return local_pckg_index_name

        pckg_index_name, pckg_index_file = find_pckg_index(self.pckg_dir)
        pckg_archive_name = find_pckg_archive_name(self.pckg_dir)

        self.local_pckg_index = load_pckg_index(pckg_index_file)
        set_local_url(self.local_pckg_index, self.server_url, pckg_archive_name)
        self.local_pckg_index_name = save_local_index(
            self.local_pckg_index, pckg_index_name
        )

    def __get_core_packager_name(self):
        """
        Returns the core package name and architecture
        which is required by the "arduino-cli" core commands with format:

            <package-name>:<architecture>
        """
        packager_name = self.local_pckg_index["packages"][0]["name"]
        core_name = self.local_pckg_index["packages"][0]["platforms"][0]["architecture"]

        return packager_name + ":" + core_name

    def __core_uninstall(self):
        """
        Uninstalls the core package if it is already installed using
        the "arduino-cli" core commands
        """
        pckgr_core_name = self.__get_core_packager_name()
        try:
            logging.info(f'Uninstalling core package "{pckgr_core_name}"')
            stdout, stderr = subprocess.run(
                ["arduino-cli", "core", "uninstall", pckgr_core_name], check=True
            )

        except subprocess.CalledProcessError as e:
            # Error code 7 means the package is not installed
            if e.returncode == 7:
                # If the package is not installed, then continue
                logging.info(f'Core package "{pckgr_core_name}" not installed')
                return

            # Print the stderror output of the subprocess
            logging.error(f'Failed to uninstall core package "{pckgr_core_name}"')
            logging.error(f"Error: {stderr}")
            sys.exit(1)

    def __core_install(self):
        """
        Installs the core package using the "arduino-cli" core commands
        """
        pckgr_core_name = self.__get_core_packager_name()

        try:
            local_pckg_index_url = os.path.join(
                self.server_url, self.local_pckg_index_name
            )
            logging.info(f'Installing core package from url "{local_pckg_index_url}"')
            subprocess.run(["arduino-cli", "core", "update-index"], check=True)
            subprocess.run(
                [
                    "arduino-cli",
                    "core",
                    "install",
                    pckgr_core_name,
                    "--additional-urls",
                    local_pckg_index_url,
                ],
                check=True,
            )
            subprocess.run(["arduino-cli", "core", "list"], check=True)
        except subprocess.CalledProcessError as e:
            logging.error(f"Failed to install core package")
            logging.error(f"Error: {e}")
            sys.exit(1)


def pckg_local_install(pckg_dir):
    """
    Installs the package from local index

    It requires the package directory to be hosted on a local server

    Args:
        - pckg_dir: Path to the package directory
    """
    server = LocalServer(pckg_dir)

    PckgLocalInstaller(pckg_dir, server.get_url())

    del server


class PckgLocalInstallParser:
    """
    Parser for the local package installer tool
    """

    def __init__(self):
        """
        Creates the parser for the local package installer tool and
        parses the command line arguments
        """
        # Get the script name without .py extension
        self.installer_tool_name = os.path.splitext(os.path.basename(__file__))[0]
        self.installer_tool_version = "0.1.0"
        self.__create_parser()

        args = self.parser.parse_args(
            namespace=argparse.Namespace(installer_parser=self)
        )
        args.func(args)

    """ Private class methods """

    class __ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string=None):
            # Retrieve the package_parser object from the namespace
            installer_parser = getattr(namespace, "installer_parser", None)
            print(
                installer_parser.installer_tool_name
                + " version: "
                + installer_parser.installer_tool_version
            )
            parser.exit()

    def __main_parser_func(self, args):
        """
        Installs the package from local index
        """
        if args.verbose:
            # Enable logging info
            logging.getLogger().setLevel(logging.INFO)

        pckg_local_install(args.pckg_dir)

    def __create_parser(self):
        """
        Creates the parser for the local package installer tool
        """
        self.parser = argparse.ArgumentParser(
            description="Arduino CLI local package installer tool"
        )

        # Argument for version
        self.parser.add_argument(
            "-v",
            "--version",
            action=self.__ver_action,
            help=self.installer_tool_name + " version",
        )

        # Argument for package location
        self.parser.add_argument(
            "--pckg-dir",
            type=str,
            default=os.getcwd(),
            help="Path to the package directory. Default is the current directory",
        )

        # Argument for verbose output
        self.parser.add_argument(
            "--verbose",
            action="store_true",
            default=False,
            help="Verbose output",
        )

        # Set the main parser function
        self.parser.set_defaults(func=self.__main_parser_func)


if __name__ == "__main__":

    pckg_install_parser = PckgLocalInstallParser()
