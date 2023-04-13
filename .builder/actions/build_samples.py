import Builder
import os
import sys
import argparse


class BuildSamples(Builder.Action):
    def run(self, env):
        # parse extra cmake configs
        parser = argparse.ArgumentParser()
        parser.add_argument('--cmake-extra', action='append', default=[])
        cmd_args = parser.parse_known_args(env.args.args)[0]

        steps = []
        samples = [
            'samples/greengrass/basic_discovery',
            'samples/greengrass/ipc',
            'samples/fleet_provisioning/fleet_provisioning',
            'samples/jobs/describe_job_execution',
            'samples/mqtt/basic_connect',
            'samples/mqtt/custom_authorizer_connect',
            'samples/mqtt/pkcs11_connect',
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
            'samples/shadow/shadow_sync',
        ]

        defender_samples = []
        # Linux only builds
        if sys.platform == "linux" or sys.platform == "linux2":
            defender_samples.append('samples/device_defender/basic_report')

        da_samples = [
            'deviceadvisor/tests/mqtt_connect',
            'deviceadvisor/tests/mqtt_publish',
            'deviceadvisor/tests/mqtt_subscribe',
            'deviceadvisor/tests/shadow_update'
        ]

        for sample_path in samples:
            build_path = os.path.join('build', sample_path)
            steps.append(['cmake',
                          f'-B{build_path}',
                          f'-H{sample_path}',
                          f'-DCMAKE_PREFIX_PATH={env.install_dir}',
                          '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
            # append extra cmake configs
            steps[-1].extend(cmd_args.cmake_extra)
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        for sample_path in da_samples:
            build_path = os.path.join('build', sample_path)
            steps.append(['cmake',
                          f'-B{build_path}',
                          f'-H{sample_path}',
                          f'-DCMAKE_PREFIX_PATH={env.install_dir}',
                          '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
            # append extra cmake configs
            steps[-1].extend(cmd_args.cmake_extra)
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        for sample_path in defender_samples:
            build_path = os.path.join('build', sample_path)
            steps.append(['cmake',
                          f'-B{build_path}',
                          f'-H{sample_path}',
                          f'-DCMAKE_PREFIX_PATH={env.install_dir}',
                          '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        return Builder.Script(steps)
