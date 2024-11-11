import Builder

class SdkCiPrep(Builder.Action):
    def run(self, env):
        actions = [
            Builder.SetupCrossCICrtEnvironment()
        ]
        return Builder.Script(actions, name='sdk-ci-prep')
