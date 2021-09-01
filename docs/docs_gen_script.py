import os

if os.system("doxygen ./doxygen.config") != 0:
    print("The 'doxygen' command was not found. Make sure you have Doxygen install.")
    exit(1)
exit(0)
