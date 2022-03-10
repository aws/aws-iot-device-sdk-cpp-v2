import Builder
import os
import sys


class BuildSamples(Builder.Action):
    def run(self, env):
        if env.args.cli_config['variables'].get('build_da', "0") == "0":
            print('skip_da is not defined. Skipping device advisor samples...')
            return

        steps = []
        samples = [
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
            steps.append(['cmake',
                          '--build', build_path,
                          '--config', 'RelWithDebInfo'])

        return Builder.Script(steps)
