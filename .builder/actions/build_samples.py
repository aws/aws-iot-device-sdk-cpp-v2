import Builder
import os
import sys


class BuildSamples(Builder.Action):
    def run(self, env):
        if env.args.cli_config['variables'].get('skip_samples', "0") != "0":
            print('skip_samples is defined. Skipping samples...')
            return

        steps = []
        samples = [
            'samples/pub_sub/basic_pub_sub',
            'samples/mqtt/basic_connect',
            'samples/mqtt/pkcs11_connect',
            'samples/mqtt/raw_connect',
            'samples/mqtt/websocket_connect',
            'samples/mqtt/x509_credentials_provider_connect',
            'samples/shadow/shadow_sync',
            'samples/greengrass/basic_discovery',
            'samples/identity/fleet_provisioning',
            'samples/jobs/describe_job_execution',
            'samples/secure_tunneling/secure_tunnel',
            'samples/secure_tunneling/tunnel_notification',
        ]
        for sample_path in samples:
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
