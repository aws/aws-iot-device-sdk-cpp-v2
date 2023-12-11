
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.

import argparse
import uuid
import os
import sys
import run_in_ci
import ci_iot_thing


def main():
    argument_parser = argparse.ArgumentParser(
        description="Run Fleet Provisioning test in CI")
    argument_parser.add_argument(
        "--config-file", required=True,
        help="JSON file providing command-line arguments to a test")
    argument_parser.add_argument(
        "--region", required=False, default="us-east-1", help="The name of the region to use")
    argument_parser.add_argument(
        "--input-uuid", required=False, help="UUID for thing name. UUID will be generated if this option is omit")
    parsed_commands = argument_parser.parse_args()
    # input_uuid = parsed_commands.input_uuid if parsed_commands.input_uuid else str(uuid.uuid4())

    # Perform fleet provisioning. If it's successful, a newly created thing should appear.

    try:
        test_result = run_in_ci.setup_and_launch(parsed_commands.config_file)
    except Exception as e:
        print(f"ERROR: Failed to execute Shared Subscription test: {e}")
        test_result = -1

    if test_result != 0:
        sys.exit(-1)


if __name__ == "__main__":
    main()
