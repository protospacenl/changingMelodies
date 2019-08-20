import serial
import time
from gpiozero import DigitalOutputDevice
import RPi.GPIO as GPIO

from cmrobot.joint import JointAX, JointMX
from cmrobot.controller import Controller

class Robot():
    def __init__(self, controller=None, sensors=None, pump="GPIO4", start_button=None):
        self.__controller = Controller(**controller)
        self.__sensors = {}
        self.__gpio = {}
        self.__pump = None
        self.__start_button = None
        self.__id_map = [None] * 255
        self.__joints = {}

        GPIO.setmode(GPIO.BCM)
        self.__init_sensors(sensors, start_button)
        self.__init_pump(pump)


    def __init_sensors(self, sensors, start_button):
        if sensors == None:
            return

        for s in sensors:
            pull = s['pull_up'] if 'pull_up' in s else None
            bounce_time = s['bounce_time'] if 'bounce_time' in s else 200
            active_state= s['active_state'] if 'active_state' in s else True

            if pull == True:
                pull = GPIO.PUD_UP
            elif pull == False:
                pull = GPIO.PUD_DOWN

            #gpio = Button(s['gpio'], pull_up=pull_up, bounce_time=bounce_time, active_state=active_state)
            gpio = None
            if pull:
                gpio = GPIO.setup(s['gpio'], GPIO.IN, pull)
            else:
                gpio = GPIO.setup(s['gpio'], GPIO.IN)

            print(f"Added GPIO {s['name']}: {gpio!r}, bounce_time: {bounce_time}")
            self.__sensors[s['name']] = gpio

            if start_button == s['name']:
                self.__start_button = s['gpio']
                #GPIO.add_event_detect(self.__start_button, GPIO.FALLING if not active_state else GPIO.RISING, bouncetime=bounce_time)
        

    def __init_pump(self, pump):
        self.__pump = DigitalOutputDevice(pump)

    def get_sensor(self, name):
        if name in self.__sensors:
            return self.__sensors[name]
        return None

    @property
    def pump(self):
        return self.__pump

    @property
    def start_button(self):
        return self.__start_button

    @property
    def start_button_state(self):
        return GPIO.input(self.__start_button)

    def start_button_event_detected(self):
        return GPIO.event_detected(self.__start_button)

    def start_button_wait_for_press(self):
        GPIO.wait_for_edge(self.__start_button, GPIO.FALLING)

    @property
    def joints(self):
        return list(self.__joints.keys())

    @property
    def joint_ids(self):
        return [ self.__joints[j].id for j in self.__joints ]

    def get_joint_by_name(self, name):
        if name in self.__joints:
            return self.__joints[name]
        return None

    def get_joint_by_id(self, id):
        return(self.__id_map[id])
   
    def connect(self):
        return self.__controller.connect()

    def add_joints(self, joints):
        for j in joints:
            self.add_joint(j)
            
    def add_joint(self, joint):
        if not 'name' in joint.keys():
            print(f"Joint has no name, skipping {joint!r}")
            return None

        if joint['name'] in self.__joints:
            print(f"Warning joint exists, overwriting!")

        nj = None

        if joint['type'].lower() == 'ax':
            nj = JointAX(**joint, controller=self.__controller)
        elif joint['type'].lower() == 'mx':
            nj = JointMX(**joint, controller=self.__controller)

        P = 0 
        if nj.type == 'MX' and 'PGain' in nj.params:
            P = nj.params['PGain']

        self.__controller.add_servo(nj.id, nj.type_id, P)
        self.__joints[nj.name] = nj
        self.__id_map[nj.id] = nj
        
        return nj

    def move_joint_to(self, name, position, speed=0, torque=0):
        if not name in self.joints:
            return
        j = self.__joints[name]
        j.move_to(position, speed=speed, torque=torque)
    
    def move_joints_to(self, positions, speed=0, torque=0, wait=0):
        for p in positions:
            _s = speed
            _t = torque
            if 'speed' in p:
                _s = p['speed']
            if 'torque' in p:
                _t = p['torque']
            self.move_joint_to(p['name'], p['pos'], speed=_s, torque=_t)

        if wait > 0:
            time.sleep(wait)

    def relax_joint(self, name):
        if not name in self.joints:
            return
        self.__joints[name].relax()

    def relax_all(self):
        for j in self.joints:
            self.relax_joint(j)

    def hold_joint(self, name):
        if not name in self.joints:
            return
        self.__joints[name].hold()
    
    def hold_all(self):
        for j in self.joints:
            self.hold_joint(j)

    def tool_home(self):
        self.__controller.tool_home()

    def get_positions(self):
        positions = self.__controller.get_positions()
        return { self.get_joint_by_id(p[0]).name:p[1] for p in positions }

    def report(self):
        print("\n************** SERVO REPORT *************\n")
        for j in self.joints:
            self.__controller.report(j, self.__joints[j].id)
        print("\n************** ***** ****** *************\n")

    def move_tool_to(self, position, speed=10):
        self.__controller.write_tool_pos(position['x'], position['z'], speed)
