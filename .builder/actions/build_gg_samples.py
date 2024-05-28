# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.

import Builder
import os


class BuildGGSamples(Builder.Action):
    """
    Defines a custom build step for Greengrass samples.
    It's used via builder.json config file in the project root directory.
    """

    def run(self, env):
        # parse extra cmake configs
        cmd_args = env.args

        steps = []
        samples = [
            'samples/greengrass/ipc',
            'samples/greengrass/basic_discovery',
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

        return Builder.Script(steps)
