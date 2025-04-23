import atexit
import Builder
import os
import subprocess

class SetupEventstreamServer(Builder.Action):

    def _build_and_run_eventstream_echo_server(self, env):
        java_sdk_dir = None

        try:
            env.shell.exec(["mvn", "--version"], check=True)

            # maven is installed, so this is a configuration we can start an event stream echo server
            java_sdk_dir = env.shell.mktemp()

            env.shell.exec(["git", "clone", "https://github.com/aws/aws-iot-device-sdk-java-v2"], working_dir=java_sdk_dir, check=True)

            sdk_dir = os.path.join(java_sdk_dir, "aws-iot-device-sdk-java-v2", "sdk")
            env.shell.pushd(sdk_dir)

            try:
                # The EchoTest server is in test-only code
                env.shell.exec(["mvn", "-q", "test-compile"], check=True)

                env.shell.exec(["mvn", "-q", "dependency:build-classpath", "-Dmdep.outputFile=classpath.txt"], check=True)

                with open('classpath.txt', 'r') as file:
                    classpath = file.read()

                test_class_path = os.path.join(sdk_dir, "target", "test-classes")
                target_class_path = os.path.join(sdk_dir, "target", "classes")
                directory_separator = os.pathsep

                echo_server_probe_command = [
                    "java",
                    "-classpath",
                    f"{test_class_path}{directory_separator}{target_class_path}{directory_separator}{classpath}",
                    "software.amazon.awssdk.eventstreamrpc.echotest.EchoTestServiceRunner"]

                """
                Try to run the echo server in the foreground without required arguments.  This always fails, but
                the output can tell us whether or not the Java CRT is available on the platform (we have SDK CI
                that runs on platforms that the Java CRT does not support).
                
                If the CRT supports the platform, we fail with an out-of-index exception (referencing non-existent
                command line arguments)
                
                If the CRT does not support the platform, we fail with 
                'java.io.IOException: Unable to open library in jar for AWS CRT'
                """
                probe_output = ""
                probe = subprocess.Popen(
                    echo_server_probe_command,
                    stdout=subprocess.STDOUT,
                    stderr=subprocess.PIPE,
                    shell=True,
                    bufsize=0)  # do not buffer output
                with probe:

                    # Convert all output to strings, which makes it much easier to both print
                    # and process, since all known uses of parsing output want strings anyway
                    line = probe.stderr.readline()
                    while (line):
                        # ignore weird characters coming back from the shell (colors, etc)
                        if not isinstance(line, str):
                            line = line.decode('ascii', 'ignore')
                        # We're reading in binary mode, so no automatic newline translation
                        if sys.platform == 'win32':
                            line = line.replace('\r\n', '\n')
                        probe_output += line
                        line = probe.stderr.readline()
                    probe.wait()

                print("Probe stderr:\n\n")
                print(probe_output)

                if "java.io.IOException" in probe_output:
                    print("Skipping eventstream server unsupported platform")
                    raise Exception("Java CRT not supported by this platform")

                echo_server_command = [
                    "java",
                    "-Daws.crt.log.level=Trace",
                    "-Daws.crt.log.destination=File",
                    "-Daws.crt.log.filename=/tmp/crt.txt",
                    "-classpath",
                    f"{test_class_path}{directory_separator}{target_class_path}{directory_separator}{classpath}",
                    "software.amazon.awssdk.eventstreamrpc.echotest.EchoTestServiceRunner",
                    "127.0.0.1",
                    "8033"]

                print(f'Echo server command: {echo_server_command}')

                # bypass builder's exec wrapper since it doesn't allow for background execution
                proc = subprocess.Popen(echo_server_command)

                @atexit.register
                def _terminate_echo_server():
                    proc.terminate()
                    proc.wait()
                    with open('/tmp/crt.txt', 'r') as logfile:
                        serverlog = logfile.read()
                        print("**************************************************")
                        print("Eventstream Server Log:\n\n")
                        print(serverlog)
                        print("\n\nEnd Log")
                        print("**************************************************")
                        os.remove("/tmp/crt.txt")

                env.shell.setenv("AWS_TEST_EVENT_STREAM_ECHO_SERVER_HOST", "127.0.0.1", quiet=False)
                env.shell.setenv("AWS_TEST_EVENT_STREAM_ECHO_SERVER_PORT", "8033", quiet=False)
            finally:
                env.shell.popd()

        except Exception as ex:
            print('Failed to set up event stream server.  Eventstream CI tests will not be run.')
            print(ex)

        return java_sdk_dir

    def run(self, env):

        actions = []
        java_sdk_dir = None

        try:
            java_sdk_dir = self._build_and_run_eventstream_echo_server(env)
            Builder.SetupCrossCICrtEnvironment().run(env)
        except:
            if java_sdk_dir:
                env.shell.rm(java_sdk_dir)

        return Builder.Script(actions, name='setup-eventstream-server')
