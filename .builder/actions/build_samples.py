import Builder
import os
import sys
import argparse


class BuildSamples(Builder.Action):
    def run(self, env):
        if env.args.cli_config['variables'].get('skip_samples', "0") != "0":
            print('skip_samples is defined. Skipping samples...')
            return

        # test to see if the args are passed in
        parser = argparse.ArgumentParser()
        parser.add_argument('--cmake-extra', action='append', default=[])
        cmd_args = parser.parse_known_args(env.args.args)[0]

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
        for sample_path in samples:
            build_path = os.path.join('build', sample_path)
            steps.append(['cmake',
                          f'-B{build_path}',
                          f'-H{sample_path}',
                          f'-DCMAKE_PREFIX_PATH={env.install_dir}',
                          '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
            for cmake_step in cmd_args.cmake_extra:
                steps[-1].append(cmake_step)
            print(steps[-1])
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        return Builder.Script(steps)
