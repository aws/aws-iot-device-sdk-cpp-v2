# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.

import Builder
import itertools
import os
import sys


class BuildSamples(Builder.Action):
    """
    Defines a custom build step for samples and tests.
    It's used via builder.json config file in the project root directory.
    """

    def run(self, env):
        # parse extra cmake configs
        cmd_args = env.args

        steps = []
        samples = [
            'samples/deprecated/fleet_provisioning/fleet_provisioning',
            'samples/deprecated/fleet_provisioning/mqtt5_fleet_provisioning',
            'samples/deprecated/jobs/job_execution',
            'samples/deprecated/jobs/mqtt5_job_execution',
            'samples/deprecated/shadow/shadow_sync',
            'samples/deprecated/shadow/mqtt5_shadow_sync',
            'samples/greengrass/basic_discovery',
            'samples/greengrass/ipc',
            'samples/fleet_provisioning/provision-basic',
            'samples/fleet_provisioning/provision-csr',
            'samples/jobs/jobs-sandbox',
            'samples/mqtt/basic_connect',
            'samples/mqtt/custom_authorizer_connect',
            'samples/mqtt/pkcs11_connect',
            'samples/mqtt/pkcs12_connect',
            'samples/mqtt/websocket_connect',
            'samples/mqtt/windows_cert_connect',
            'samples/mqtt/x509_credentials_provider_connect',
            'samples/mqtt/custom_authorizer_connect',
            'samples/mqtt/cognito_connect',
            'samples/mqtt5/mqtt5_pubsub',
            'samples/mqtt5/mqtt5_shared_subscription',
            "samples/pub_sub/basic_pub_sub",
            "samples/pub_sub/cycle_pub_sub",
            'samples/secure_tunneling/secure_tunnel',
            'samples/secure_tunneling/tunnel_notification',
            'samples/shadow/shadow-sandbox',
        ]

        defender_samples = []
        # Linux only builds
        if sys.platform == "linux" or sys.platform == "linux2":
            defender_samples.append('samples/device_defender/basic_report')
            defender_samples.append('samples/device_defender/mqtt5_basic_report')

        da_samples = [
            'deviceadvisor/tests/mqtt_connect',
            'deviceadvisor/tests/mqtt_publish',
            'deviceadvisor/tests/mqtt_subscribe',
            'deviceadvisor/tests/shadow_update'
        ]

        servicetests = [
            'servicetests/tests/JobsExecution/',
            'servicetests/tests/FleetProvisioning/',
            'servicetests/tests/ShadowUpdate/',
        ]

        for sample_path in itertools.chain(samples, servicetests, da_samples, defender_samples):
            build_path = os.path.join('build', sample_path)
            steps.append(['cmake',
                          f'-B{build_path}',
                          f'-H{sample_path}',
                          f'-DCMAKE_PREFIX_PATH={env.install_dir}',
                          '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
            # append extra cmake configs
            steps[-1].extend(cmd_args.cmake_extra)
            # Currently, cmake_args sets only Linux-specific options.
            if sys.platform == "linux" or sys.platform == "linux2":
                steps[-1].extend(env.config['cmake_args'])
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        return Builder.Script(steps)
