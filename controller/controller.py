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

PLAYLIST_TEMPLATE = { 
        'positions': { 
            'arm': {}, 
            'tool': {} 
        }, 
        'playlist': [ 
            { "target": "touch", 
              "cmd": "_wait_for_trigger", 
              "timeout": 10, 
              "on_timeout": "restart", 
              "#": "Wait for trigger"
            }
        ]
    }

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
    if cmd['joint'] == 'all':
        robot.hold_all()
    else:
        robot.hold_joint(['joint'])

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
            'head': handle_move_arm,
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
    interactive     = False

    try:
        opts, args = getopt.getopt(argv[1:], "hc:p:i", ["config=","playlist=","interactive"])
    except getopt.GetoptError:
        print('{} -c <config> -p <playlist> -o <path> -i'.format(sys.argv[0]))
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('{} -c <config> -p <playlist> -m'.format(sys.argv[0]))
            sys.exit()
        elif opt in ("-c", "--config"):
            config_path = arg
        elif opt in ("-p", "--playlist"):
            playlist_path = arg
        elif opt in ("-i", "--interactive"):
            interactive = True

    with open(config_path, 'r') as f:
        config = json.load(f)

    with open(playlist_path, 'r') as f:
        playlist = json.load(f)


    mixer.init(frequency=8000)

    robot_config = config['robot']
    try:
        head_config = config['head']
    except KeyError:
        print("No head")
        head_config = None

    robot = Robot(**config['robot'])
    robot.connect()

    joints = config['joints']
    robot.add_joints(joints)

    arm_positions = playlist['positions']['arm']
    if 'head' in playlist['positions']:
        head_positions = playlist['positions']['head']
    tool_positions = playlist['positions']['tool']
    playlist = playlist['playlist']

    print(f"Created robot with joints {robot.joints}")

    if head_config:
        head = serial.Serial(head_config['port'], head_config['baudrate'], timeout=head_config['timeout'])
        print(f"Opened connection to the head")


    if interactive:
        joint_select_str = ""
        selected_joints = []
        last_positions = None 
        outpath = None

        robot.relax_all()

        printHelp():
            print("Interactive mode:")
            print("\t?: help")
            print("\th: hold")
            print("\tr: release")
            print("\tp: print positions")
            print("\tn: new file for saving positions (current: {}".format(outpath))
            print("\ts: save positions file given with -o or n command")
            print("\tq: quit")
            print("")

        printHelp()

        for k in robot.joints:
            j = robot.get_joint_by_name(k):
                joint_select_str = s + "{}: {}, ".format(j.id, j.name)
        s = s[:-2]

        selected_joints = robot.joint_ids

        while True:
            cmd = input("Command: ").lower()
            if cmd == 'h':
                print('Holding')
                robot.hold_all()
            elif cmd == 'r':
                print('Relaxing')
                robot.relax_all()
            elif cmd == 'p':
                last_positions = robot.get_positions()
                print("positions: {}".format(last_positions))
            elif cmd == 'n':
                outpath = input("filename: ")
                with open(outpath, 'w') as out:
                    json.dump(PLAYLIST_TEMPLATE, out, indent=4)

            elif cmd == 's':
                _playlist = None

                if not outpath:
                    print("not output file")
                    continue

                with open(outpath, 'r') as out:
                    _playlist = json.load(out)

                if not last_positions:
                    last_positions = robot.get_positions()

                print("positions: {}".format(last_positions))
                
                name = input("name: ")
                joints = input("joints (0 for all) [{}]\nCurrent: {}\n> ".format(joint_select_str, selected_joints))
                
                if joints != "":
                    joints = [ int(joint) for joint in joints ]
                    if 0 in joints:
                        selected_joints = robot.joint_ids
                    else:
                        selected_joints = joints
                
                joints = []
                for i in selected_joints:
                    jointname = robot.get_joint_by_id(i)
                    joints.append({'name': jointname, 'pos': last_positions[jointname], 'torque': 200})
                _playlist['positions']['arm']['name'] = { 'joints': joints }
                with open(outpath, 'w') as out:
                    json.dump(_playlist, out, indent=4)

                print("Saved positions")

            elif cmd == 'q':
                robot.relax_all()
                sys.exit(1)
            elif cmd == '?':
                printHelp()
            else:
               print('unkown command')
               printHelp()

    try:
        start_time      = datetime.now()
        delay_start     = None
        delay_for_s     = None
        wait_for_delay  = False

        playlist_iterator = iter(playlist)

        while True:
            if not wait_for_delay:
                _ = None
                try:
                    _ = next(playlist_iterator)
                    cmd = _['cmd']
                    target = _['target'] if 'target' in _ else None

                    if cmd in CMD_HANDLER_MAP.keys():
                        if cmd == 'move':
                            if target == 'arm':
                                CMD_HANDLER_MAP[cmd][target](robot, arm_positions, _)
                            if target == 'head':
                                CMD_HANDLER_MAP[cmd][target](robot, head_positions, _)
                            elif target == 'tool':
                                CMD_HANDLER_MAP[cmd][target](robot, tool_positions, _)
                        elif cmd == 'relax':
                            CMD_HANDLER_MAP[cmd](robot, _)
                        elif cmd == 'say':
                            CMD_HANDLER_MAP[cmd](robot, _)
                        elif cmd == 'delay':
                            #CMD_HANDLER_MAP[cmd](robot, _)
                            delay_start = datetime.now()
                            delay_for_s = _['seconds']
                            wait_for_delay = True
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

                except StopIteration as e:
                    print(f"{e!r}")
                    playlist_iterator = iter(playlist)

            else:
                if (datetime.now() - delay_start).seconds >= delay_for_s:
                    wait_for_delay = False
            t2 = datetime.now()
            if (t2 - start_time).seconds >= 0.1:
                robot.report()
                start_time = t2

    except KeyboardInterrupt:
        #242 2254 462
        robot.relax_all()

    close(outfile)

if __name__ == "__main__":
    main(sys.argv)
