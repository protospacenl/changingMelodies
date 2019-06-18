import serial
import time
from gpiozero import Button

from cmrobot.joint import JointAX, JointMX
from cmrobot.controller import Controller

class Robot():
    def __init__(self, controller=None, sensors=None):
        self.__controller = Controller(**controller)
        self.__sensors = {}
        self.__joints = {}

        self.__init_sensors(sensors)

    def __init_sensors(self, sensors):
        if sensors == None:
            return

        for s in sensors:
            pull_up = s['pull_up'] if 'pull_up' in s else None
            bounce_time = s['bounce_time'] if 'bounce_time' in s else None
            active_state= s['active_state'] if 'active_state' in s else None
            gpio = Button(s['gpio'], pull_up=pull_up, bounce_time=bounce_time, active_state=active_state)
            print(f"Added GPIO {s['name']}: {gpio!r}, bounce_time: {gpio.pin.bounce}")
            self.__sensors[s['name']] = gpio

    def get_sensor(self, name):
        if name in self.__sensors:
            return self.__sensors[name]

    @property
    def joints(self):
        return list(self.__joints.keys())
   
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

        self.__controller.add_servo(nj.id, nj.type_id)
        self.__joints[nj.name] = nj
        
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

    def hold(self):
        self.__controller.hold()
    
    def tool_home(self):
        self.__controller.tool_home()

    def monitor(self):
        self.__controller.monitor()

    def report(self):
        print("\n************** SERVO REPORT *************\n")
        for j in self.joints:
            self.__controller.report(j, self.__joints[j].id)
        print("\n************** ***** ****** *************\n")

    def move_tool_to(self, position, speed=10):
        self.__controller.write_tool_pos(position['x'], position['z'], speed)


