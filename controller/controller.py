import sys
import getopt
import json
import operator
import time

from pathlib import Path
from time import sleep

from cmrobot import Robot

CONFIG_PATH = './config.json'

def handle_move_arm(robot, positions, cmd):
    if cmd['position'] in positions:
        p = positions[cmd['position']]['joints']
        robot.move_joints_to(p)

def handle_move_tool(robot, positions, cmd):
    if cmd['position'] in positions:
        p = positions[cmd['position']]
        print(f"Movie tool to position: {p!r}") 

def handle_relax(robot, cmd):
    if cmd['joint'] == 'all':
        robot.relax_all()
    else:
        robot.relax_joint(cmd['joint'])

def handle_say(cmd):
    print("say")

def handle_delay(cmd):
    print(f"delay {cmd['seconds']}")
    time.sleep(cmd['seconds'])

CMD_HANDLER_MAP = {
        'move': { 
            'arm': handle_move_arm,
            'tool': handle_move_tool
        },
        'relax': handle_relax,
        'say': handle_say,
        'delay': handle_delay
    }

def main(argv):
    config_path     = CONFIG_PATH
    config          = None

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

    robot_config = config['robot']
    head_config = config['head']

    robot = Robot(**config['robot'])
    robot.connect()

    joints = config['joints']
    robot.add_joints(joints)

    sensors = config['sensors']

    arm_positions = config['positions']['arm']
    tool_positions = config['positions']['tool']
    playlist = config['playlist']

    print(f"Created robot with joints {robot.joints}")

    for _ in playlist:
        cmd = _['cmd']
        target = _['target']
        if cmd in CMD_HANDLER_MAP.keys():
            if cmd == 'move':
                if target == 'arm':
                    CMD_HANDLER_MAP[cmd][target](robot, arm_positions, _)
                elif target == 'tool':
                    CMD_HANDLER_MAP[cmd][target](robot, tool_positions, _)
            elif cmd == 'relax':
                CMD_HANDLER_MAP[cmd](robot, _)
            elif cmd == 'say':
                CMD_HANDLER_MAP[cmd](_)
            elif cmd == 'delay':
                CMD_HANDLER_MAP[cmd](_)
                 


if __name__ == "__main__":
    main(sys.argv)
