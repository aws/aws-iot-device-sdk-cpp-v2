import Builder

class SdkCiTestSetup(Builder.Action):

    def run(self, env):

        actions = []
        java_sdk_dir = None

        try:
            java_sdk_dir = Builder.SetupEventStreamEchoServer().run(env)
            Builder.SetupCrossCICrtEnvironment().run(env)
        except Exception as ex:
            print(f'Failure while setting up tests: {ex}')
            actions.append("exit 1")
        finally:
            if java_sdk_dir:
                env.shell.rm(java_sdk_dir)

        return Builder.Script(actions, name='sdk-ci-test-setup')
