from asyncio import subprocess
import os
import os.path
import shutil
import subprocess

# Place in the sample folder to be built
# Change DCMAKE_PREFIX_PATH below to the <absolute path sdk-cpp-workspace dir>

root_dir = os.path.abspath(os.path.dirname(__file__))
build_dir = os.path.join(root_dir, 'build')

# Delete existing Sample build directory
shutil.rmtree(build_dir, ignore_errors=True)
os.mkdir(build_dir)

os.chdir(build_dir)


def check_call(args):
    print('$', subprocess.list2cmdline(args))
    subprocess.check_call(args)


# Build
check_call(['cmake', '-DCMAKE_PREFIX_PATH="/Users/sbstevek/workplace/secure-tunnel-multiplexing/installs"',
           '-DCMAKE_BUILD_TYPE="Debug"', '..'])
check_call(['cmake', '--build', '.', '--config', 'Debug'])


# "cmake -DCMAKE_PREFIX_PATH="/Users/sbstevek/workplace/secure-tunnel-multiplexing/installs" -DCMAKE_BUILD_TYPE="Debug" .."
# "cmake --build . --config Debug"
