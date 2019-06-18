import sys
import getopt
import json
import operator
import time
import serial

from datetime import datetime
from pathlib import Path
from time import sleep
from pygame import mixer

from cmrobot import Robot

CONFIG_PATH = './config.json'
PLAYLIST_PATH = './playlist.json'

def handle_move_arm(robot, positions, cmd):
    if cmd['position'] in positions:
        p = positions[cmd['position']]['joints']
        speed = cmd['speed'] if 'speed' in cmd else 0
        torque = cmd['torque'] if 'torque' in cmd else 0
        wait = cmd['wait'] if 'wait' in cmd else 0
        robot.move_joints_to(p, speed=speed, torque=torque, wait=wait)

def handle_move_tool(robot, positions, cmd):
    if cmd['position'] in positions:
        speed = cmd['speed'] if 'speed' in cmd else 10
        p = positions[cmd['position']]
        print(f"Movie tool to position: {p!r} with speed {speed}") 
        robot.move_tool_to(p, speed) 


def handle_relax(robot, cmd):
    if cmd['joint'] == 'all':
        robot.relax_all()
    else:
        robot.relax_joint(cmd['joint'])

def handle_say(robot, cmd):
    path = Path(cmd['path']) if 'path' in cmd else None
    if not path or not path.exists():
        return
    print(f"say {path}")
    mixer.music.load(path.as_posix())
    mixer.music.play(1)

def handle_home(robot, cmd):
    robot.tool_home()

def handle_hold(robot, cmd):
    robot.hold()

def handle_delay(rebot, cmd):
    print(f"delay {cmd['seconds']}")
    time.sleep(cmd['seconds'])

def handle_wait_trigger(robot, cmd):
    timeout = cmd['timeout'] if 'timeout' in cmd else None
    s = robot.get_sensor(cmd['target'])
    print(f"Waiting for trigger from {s!r}")
    t = s.wait_for_press(timeout=timeout)
    print(f"Triggered: {t!r}, Value: {s.value}")
    return t


CMD_HANDLER_MAP = {
        'move': { 
            'arm': handle_move_arm,
            'tool': handle_move_tool
        },
        'relax': handle_relax,
        'say': handle_say,
        'hold': handle_hold,
        'home': handle_home,
        'delay': handle_delay,
        'wait_for_trigger': handle_wait_trigger,
        "send" : None
    }

def main(argv):
    config_path     = CONFIG_PATH
    playlist_path   = PLAYLIST_PATH
    config          = None
    playlist        = None
    head            = None
    monitor         = False

    try:
        opts, args = getopt.getopt(argv[1:], "hc:p:m", ["config=","playlist=","monitor="])
    except getopt.GetoptError:
        print('{} -c <config> -p <playlist> -m'.format(sys.argv[0]))
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('{} -c <config> -p <playlist> -m'.format(sys.argv[0]))
            sys.exit()
        elif opt in ("-c", "--config"):
            config_path = arg
        elif opt in ("-p", "--playlist"):
            playlist_path = arg
        elif opt in ("-m", "--monitor"):
            monitor = True

    with open(config_path, 'r') as f:
        config = json.load(f)

    with open(playlist_path, 'r') as f:
        playlist = json.load(f)

    mixer.init(frequency=8000)

    robot_config = config['robot']
    head_config = config['head']

    robot = Robot(**config['robot'])
    robot.connect()

    joints = config['joints']
    robot.add_joints(joints)

    arm_positions = playlist['positions']['arm']
    tool_positions = playlist['positions']['tool']
    playlist = playlist['playlist']

    print(f"Created robot with joints {robot.joints}")

    head = serial.Serial(head_config['port'], head_config['baudrate'], timeout=head_config['timeout'])
    print(f"Opened connection to the head")

    if monitor:
        robot.relax_all()
        while True:
            robot.monitor()
            time.sleep(.5)
    try:
        start_time      = datetime.now()
        while True:
            for _ in playlist:
                cmd = _['cmd']
                target = _['target'] if 'target' in _ else None
                if cmd in CMD_HANDLER_MAP.keys():
                    if cmd == 'move':
                        if target == 'arm':
                            CMD_HANDLER_MAP[cmd][target](robot, arm_positions, _)
                        elif target == 'tool':
                            CMD_HANDLER_MAP[cmd][target](robot, tool_positions, _)
                    elif cmd == 'relax':
                        CMD_HANDLER_MAP[cmd](robot, _)
                    elif cmd == 'say':
                        CMD_HANDLER_MAP[cmd](robot, _)
                    elif cmd == 'delay':
                        CMD_HANDLER_MAP[cmd](robot, _)
                    elif cmd == 'hold':
                        CMD_HANDLER_MAP[cmd](robot, _)
                    elif cmd == 'home':
                        CMD_HANDLER_MAP[cmd](robot, _)
                    elif cmd == 'send':
                        if target  == 'head':
                            print(f"\n\n\n\n\nSending data to head: ")
                            if head:
                                head.write(_['data'].encode())
                    elif cmd == 'wait_for_trigger':
                        print("Waiting for trigger")
                        triggered = CMD_HANDLER_MAP[cmd](robot, _)
                        if triggered:
                            print(f"Triggered")
                            if 'on_trigger' in _:
                                if _['on_trigger'] == 'restart':
                                    break
                        else:
                            print(f"Timed out")
                            if 'on_timeout' in _:
                                if _['on_timeout'] == 'restart':
                                    break
                t2 = datetime.now()
                if (t2 - start_time).seconds >= 1:
                    robot.report()
                    start_time = t2

    except KeyboardInterrupt:
        #242 2254 462
        robot.relax_all()

                
                      


if __name__ == "__main__":
    main(sys.argv)
