import struct

from cmrobot.controller import Controller

class Joint():
    MAX_MOVING_SPEED    = 0x3FF
    MAX_TORQUE_LIMIT    = 0x3FF

    CMD_HEADER          = b'\xFF'
    CMD_GOAL_POSITION   = b'\x1E'
    CMD_MOVING_SPEED    = b'\x20'
    CMD_TORQUE_LIMIT    = b'\x22'

    def __init__(self, name="noname", type="AX", id=0, params={}, controller=None):
        self.__name = name
        self.__type = type
        self.__id = id
        self.__params = params
        self.__controller = controller

    @property
    def id(self):
        return self.__id

    @property
    def name(self):
        return self.__name

    @property
    def type(self):
        return self.__type

    def set_goal_position(self, position):
        cmd = struct.pack('>ccBH', self.CMD_HEADER, self.CMD_GOAL_POSITION, self.id, position)
        print(f"Writing goal position: {cmd!r}")
        self.__controller.write(cmd)

    def set_moving_speed(self, speed):
        cmd = struct.pack('>ccBH', self.CMD_HEADER, self.CMD_MOVING_SPEED, self.id, speed)
        print(f"Writing moving speed: {cmd!r}")
        self.__controller.write(cmd)

    def set_torque_limit(self, torque):
        cmd = struct.pack('>ccBH', self.CMD_HEADER, self.CMD_TORQUE_LIMIT, self.id, torque)
        print(f"Writing torque limit: {cmd!r}")
        self.__controller.write(cmd)

    def move_to(self, position, speed=0, torque=0):
        if speed == 0:
            speed = self.MAX_MOVING_SPEED
        if torque == 0:
            torque = self.MAX_TORQUE_LIMIT

        print(f"moving {self.name} to {position} with speed: {speed} and torque {torque}")
        self.set_moving_speed(speed)
        self.set_torque_limit(torque)
        self.set_goal_position(position)
        
    def relax(self):
        print(f"{self.name} relaxing")
        self.set_torque_limit(0)


class JointMX(Joint):
    MAX_MOVING_SPEED    = 0x3FF
    MAX_TORQUE_LIMIT    = 0xfff

    def __init__(self, **kw):
        super(JointMX, self).__init__(**kw)

    def move_to(self, position, speed=0, torque=0):
        print("MX move")
        super(JointMX, self).move_to(position, speed, torque)

    def relax(self):
        print("MX relax")
        super(JointMX, self).relax()

class JointAX(Joint):
    def __init__(self, **kw):
        super(JointAX, self).__init__(**kw)

    def move_to(self, position, speed=0, torque=0):
        print("AX move")
        super(JointAX, self).move_to(position, speed, torque)

    def relax(self):
        print("AX relax")
        super(JointAX, self).relax()


