import Builder
import os

class SdkCiPrep(Builder.Action):
    def run(self, env):
        if os.environ.get('AWS_ACCESS_KEY_ID') != None:
            print(f"Local credentials exist")
            if os.environ.get('AWS_SESSION_TOKEN') != None:
                print(f"Local credentials are session based")
        else:
            print(f"No Local credentials")

        actions = [
            Builder.SetupCrossCICrtEnvironment()
        ]
        return Builder.Script(actions, name='sdk-ci-prep')
