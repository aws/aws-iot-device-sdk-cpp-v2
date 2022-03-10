import Builder
import os
import sys


class BuildSamples(Builder.Action):
    def run(self, env):
        skip_da = False
        skip_samples = False
        if env.args.cli_config['variables'].get('skip_da', "0") != "0":
            skip_da = True
            print('skip_da is defined. Skipping device advisor samples...')
        if env.args.cli_config['variables'].get('skip_samples', "0") != "0":
            skip_samples = True
            print('skip_samples is defined. Skipping samples...')

        steps = []
        samples = [
            'samples/mqtt/basic_pub_sub',
            'samples/mqtt/pkcs11_pub_sub',
            'samples/mqtt/raw_pub_sub',
            'samples/shadow/shadow_sync',
            'samples/greengrass/basic_discovery',
            'samples/identity/fleet_provisioning',
            'samples/jobs/describe_job_execution',
            'samples/secure_tunneling/secure_tunnel',
            'samples/secure_tunneling/tunnel_notification',
        ]
        da_samples = [
            'deviceadvisor/tests/mqtt_connect',
            'deviceadvisor/tests/mqtt_publish',
            'deviceadvisor/tests/mqtt_subscribe',
            'deviceadvisor/tests/shadow_update'
        ]

        if skip_samples == False :
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
            
        if skip_da == False :
            for sample_path in da_samples:
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
