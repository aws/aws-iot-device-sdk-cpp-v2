# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.

# Built-in
import argparse
import os
import subprocess
import pathlib
import sys
import tempfile
import appverifier_xml
# Needs to be installed via pip
import boto3  # - for launching sample


def launchSample(sample_file, sample_region, sample_secret_endpoint, sample_secret_certificate, sample_secret_private_key, sample_arguments):
    print("Attempting to get credentials from secrets using Boto3...")
    try:
        secrets_client = boto3.client(
            "secretsmanager", region_name=sample_region)
        sample_endpoint = secrets_client.get_secret_value(
            SecretId=sample_secret_endpoint)["SecretString"]
        sample_certificate = secrets_client.get_secret_value(
            SecretId=sample_secret_certificate)
        sample_private_key = secrets_client.get_secret_value(
            SecretId=sample_secret_private_key)
    except Exception: # lgtm [py/catch-base-exception]
        sys.exit("ERROR: Could not get secrets to launch sample!")

    current_folder = pathlib.Path(__file__).resolve()
    # Remove the name of the python file
    current_folder = str(current_folder).replace("appverifier_xml_util.py", "")

    print("Saving credentials to file...")
    tmp_certificate_file_path = str(current_folder) + "tmp_certificate.pem"
    tmp_private_key_path = str(current_folder) + "tmp_privatekey.pem.key"
    with open(tmp_certificate_file_path, "w") as file:
        file.write(sample_certificate["SecretString"]) # lgtm [py/clear-text-storage-sensitive-data]
    with open(tmp_private_key_path, "w") as file:
        file.write(sample_private_key["SecretString"]) # lgtm [py/clear-text-storage-sensitive-data]
    print("Saved credentials to file...")

    print("Processing arguments...")
    launch_arguments = []
    launch_arguments.append("--endpoint")
    launch_arguments.append(sample_endpoint)
    launch_arguments.append("--cert")
    launch_arguments.append(tmp_certificate_file_path)
    launch_arguments.append("--key")
    launch_arguments.append(tmp_private_key_path)
    sample_arguments_split = sample_arguments.split(" ")
    for arg in sample_arguments_split:
        launch_arguments.append(arg)

    print("Running sample...")
    exit_code = 0
    sample_return = subprocess.run(
        args=launch_arguments, executable=sample_file)
    exit_code = sample_return.returncode

    print("Deleting credentials files...")
    os.remove(tmp_certificate_file_path)
    os.remove(tmp_private_key_path)

    if (exit_code == 0):
        print("SUCCESS: Finished running sample! Exiting with success")
    else:
        print("ERROR: Sample did not return success! Exit code " + str(exit_code))
    return exit_code


def registerAppVerifier(test_executable, app_verifier_tests_list):
    arguments = ["appverif", "-enable"] + app_verifier_tests_list + ["-for", test_executable]
    print (f'Calling AppVerifier with: {subprocess.list2cmdline(arguments)}')
    # NOTE: Needs elevated permissions.
    subprocess.run(args=arguments)


def unregisterAppVerifier(test_executable):
    arguments = ["appverif", "-delete", "settings", "-for", test_executable]
    print (f'Calling AppVerifier with: {subprocess.list2cmdline(arguments)}')
    # NOTE: Needs elevated permissions.
    subprocess.run(args=arguments)


def checkAppVerifierXML(test_executable, tmp_xml_file_path):
    appverif_xml_dump_args = ["appverif", "-export", "log", "-for", test_executable, "-with", "to="+ tmp_xml_file_path]
    print (f'Calling AppVerifier with: {subprocess.list2cmdline(appverif_xml_dump_args)}')
    # NOTE: Needs elevated permissions
    subprocess.run(args=appverif_xml_dump_args)

    xml_result = appverifier_xml.parseXML(tmp_xml_file_path, True)
    if (xml_result != 0):
        print (f"ERROR: XML parse returned failure!")
    return xml_result


def booleanString(string):
    string = string.lower()
    if string not in {"false", "true"}:
        raise ValueError("Boolean is not true or false!")
    return string == "true"


def main():
    argument_parser = argparse.ArgumentParser(
        description="AppVerifier XML output util")
    argument_parser.add_argument("--sample_file", metavar="<C:\\example\\sample.exe>",
                                 required=True, default="", help="Sample to launch that AppVerifier is following")
    argument_parser.add_argument("--sample_region", metavar="<Name of region>",
                                 required=False, default="us-east-1", help="The name of the region to use for accessing secrets")
    argument_parser.add_argument("--sample_secret_endpoint", metavar="<Name of endpoint secret>",
                                 required=False, default="unit-test/endpoint", help="The name of the secret containing the endpoint")
    argument_parser.add_argument("--sample_secret_certificate", metavar="<Name of certificate secret>", required=False,
                                 default="unit-test/certificate", help="The name of the secret containing the certificate PEM file")
    argument_parser.add_argument("--sample_secret_private_key", metavar="<Name of private key secret>", required=False,
                                 default="unit-test/privatekey", help="The name of the secret containing the private key PEM file")
    argument_parser.add_argument("--sample_arguments", metavar="<Arguments here in single string!>",
                                 required=False, default="", help="Arguments to pass to sample")

    parsed_commands = argument_parser.parse_args()

    print ("Registering with AppVerifier")
    app_verifier_tests = ["Exceptions", "Handles", "Heaps", "Leak", "Locks", "Memory", "SRWLock", "Threadpool", "TLS"]
    registerAppVerifier(parsed_commands.sample_file, app_verifier_tests)

    print("Starting to launch sample...")
    sample_result = launchSample(
        parsed_commands.sample_file,
        parsed_commands.sample_region,
        parsed_commands.sample_secret_endpoint,
        parsed_commands.sample_secret_certificate,
        parsed_commands.sample_secret_private_key,
        parsed_commands.sample_arguments)

    print ("Parsing XML...")
    tmp_xml_file_path = os.path.join(tempfile.gettempdir(), "tmp.xml")
    xml_result = checkAppVerifierXML(parsed_commands.sample_file, tmp_xml_file_path)

    print ("Unregistering with AppVerifier")
    unregisterAppVerifier(parsed_commands.sample_file)

    if (xml_result != 0):
        sys.exit(xml_result)
    sys.exit(sample_result)


if __name__ == "__main__":
    main()
