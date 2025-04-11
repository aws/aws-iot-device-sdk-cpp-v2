import Builder

class SdkCiTest(Builder.Action):

    def run(self, env):

        actions = []
        java_sdk_dir = None

        try:
            java_sdk_dir = Builder.SetupEventStreamEchoServer().run(env)
            Builder.SetupCrossCICrtEnvironment().run(env)
            env.shell.exec(["make", "test"], check=True)
        except:
            print(f'Failure while running tests')
            actions.append("exit 1")
        finally:
            if java_sdk_dir:
                env.shell.rm(java_sdk_dir)

        return Builder.Script(actions, name='sdk-ci-test')
