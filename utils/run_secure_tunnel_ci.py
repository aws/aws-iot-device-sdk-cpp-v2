# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.

# Built-in
import argparse
import subprocess
import pathlib
import sys
from time import sleep
# Needs to be installed via pip
import boto3  # - for launching sample


current_folder = pathlib.Path(__file__).resolve()


def getSecretsAndLaunch(parsed_commands):
    global tmp_certificate_file_path
    global tmp_private_key_path
    exit_code = 0

    print ("Creating secure tunnel client using Boto3")
    tunnel_client = None
    try:
        tunnel_client = boto3.client("IoTSecureTunneling", region_name=parsed_commands.sample_region)
    except Exception:
        print ("Could not create tunnel client!")
        exit(-1)

    tunnel_data = None
    try:
        tunnel_client.open_tunnel()
    except Exception:
        print ("Could not open tunnel!")
        exit(-1)

    print ("Launching Secure Tunnel samples...")
    exit_code = launch_samples(parsed_commands, tunnel_data)

    print ("Closing tunnel...")
    try:
        tunnel_client.close_tunnel(tunnel_data["tunnelId"])
    except Exception:
        print ("Could not close tunnel!")
        exit(-1)

    return exit_code


def launch_samples(parsed_commands, tunnel_data):
    exit_code = 0

    # Right now secure tunneling is only in C++, so we only support C++
    launch_arguments_destination = ["test", "region", parsed_commands.sample_region, "access_token", tunnel_data["destinationAccessToken"]]
    launch_arguments_source = ["localProxyModeSource", "region", parsed_commands.sample_region, "access_token", tunnel_data["sourceAccessToken"]]

    destination_run = subprocess.Popen(args=launch_arguments_destination, executable=parsed_commands.sample_file)
    sleep(10) # Sleep to give the destination some time to run
    source_run = subprocess.Popen(args=launch_arguments_source, executable=parsed_commands.sample_file)

    # Wait for the source to finish
    source_run.wait()
    # Once finished, stop the destination run
    destination_run.kill()

    # finish!
    return exit_code


def main():
    argument_parser = argparse.ArgumentParser(
        description="Utility to run Secure Tunneling sample in CI")
    argument_parser.add_argument("--sample_file",
                                 metavar="<CPP=C:\\repository\\PubSub.exe, Java=samples/BasicPubSub, Python=PubSub.py, Javascript=C:\\samples\\node\\pub_sub>",
                                 required=True, default="", help="Sample to launch. Format varies based on programming language")
    argument_parser.add_argument("--sample_region", metavar="<Name of region>",
                                 required=True, default="us-east-1", help="The name of the region to use for accessing secrets")
    parsed_commands = argument_parser.parse_args()

    print("Starting to launch Secure Tunneling sample...")
    sample_result = getSecretsAndLaunch(parsed_commands)
    sys.exit(sample_result)


if __name__ == "__main__":
    main()
