import Builder
import os

class SdkCiPrep(Builder.Action):
    def run(self, env):
        if os.environ.get('AWS_ACCESS_KEY_ID') != None:
            print(f"Local credentials exist")
        else:
            print(f"No Local credentials")
        actions = [
            Builder.SetupCrossCICrtEnvironment()
        ]
        return Builder.Script(actions, name='sdk-ci-prep')
