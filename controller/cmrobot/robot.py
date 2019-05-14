import serial

from cmrobot.joint import JointAX, JointMX
from cmrobot.controller import Controller

class Robot():
    def __init__(self, controller=None, joints=None):
        self.__controller = Controller(**controller)
        self.__joints = {}
        self.add_joints(joints)

    @property
    def joints(self):
        return list(self.__joints.keys())
    
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

        self.__joints[nj.name] = nj
        
        return nj

    def move_joint_to(self, name, position, speed=0, torque=0):
        if not name in self.joints:
            return
        j = self.__joints[name]
        j.move_to(position, speed=speed, torque=torque)
    
    def move_joints_to(self, positions, speed=0, torque=0):
        for p in positions:
            _s = speed
            _t = torque
            if 'speed' in p:
                _s = p['speed']
            if 'torque' in p:
                _t = p['torque']
            self.move_joint_to(p['name'], p['pos'], speed=_s, torque=_t)

    def relax_joint(self, name):
        if not name in self.joints:
            return
        self.__joints[name].relax()

    def relax_all(self):
        for j in self.joints:
            self.relax_joint(j)


