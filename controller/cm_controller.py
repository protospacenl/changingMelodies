import sys
import getopt
import json
import operator

from pathlib import Path
from time import sleep

from cmrobot import Robot

CONFIG_PATH = '/etc/cm/robot.json'

def main(argv):
    config_path     = CONFIG_PATH

    try:
        opts, args = getopt.getopt(argv[1:], "hc:", ["config="])
    except getopt.GetoptError:
        print('{} -c <config>'.format(sys.argv[0]))
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('{} -c <config>'.format(sys.argv[0]))
            sys.exit()
        elif opt in ("-c", "--config"):
            config_path = arg

    with open(config_path, 'r') as f:
        config = json.load(f)

    print("{}".format(json.dumps(config, indent=2)))

if __name__ == "__main__":
    main(sys.argv)
