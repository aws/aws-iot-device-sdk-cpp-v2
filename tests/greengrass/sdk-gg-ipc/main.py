import sys

def main():
    args = sys.argv[1:]
    print("Hello {}".format(" ".join(args)))

if __name__ == "__main__":
    main()
