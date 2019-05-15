import struct
from serial import Serial
from cmrobot.singleton import Singleton

class Controller(metaclass=Singleton):
    CMD_ACK                   = b'\xff'

    CMD_HEADER                = 0xffff
    CMD_ADD_SERVO             = 0x01
    CMD_TOOL_POSITION         = 0x02
    CMD_SERVO_GOAL_POSITION   = 0x1E
    CMD_SERVO_MOVING_SPEED    = 0x20
    CMD_SERVO_TORQUE_LIMIT    = 0x22

    def __init__(self, port="/dev/ttyUSB0", baudrate=19200, timeout=1, **kw):
        self.__port = port
        self.__baudrate = baudrate
        self.__timeout = timeout
        self.__serial = None

    def connect(self):
        self.__serial = Serial(port=self.__port, 
                               baudrate=self.__baudrate, 
                               timeout=self.__timeout)
        self.wait_for_ack()
        self.__serial.reset_input_buffer()

    def wait_for_ack(self):
        while True:
            ack = self.__serial.read(1)
            if ack == b'':
                continue
            print(f"got ack: {ack!r}")
            if ack == self.CMD_ACK:
                return True
        return False

    def write(self, data, wait=False):
        if not self.__serial:
            return None

        self.__serial.write(data)

        if wait:
            return self.wait_for_ack()

    def read(self):
        ret = self.__serial.read(1)
        print(f"Got response: {ret!r}")
        return True if ret == self.CMD_ACK else False

    def add_servo(self, id, type):
        cmd = struct.pack('HBBBB', self.CMD_HEADER, 
                                    self.CMD_ADD_SERVO, 
                                    2, id, type)
        print(f"Adding servo: {cmd!r}")
        self.write(cmd, wait=True)

    def write_joint_position(self, id, position):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_GOAL_POSITION, 3, id]))
        self.write(struct.pack('H', position), wait=True)

    def write_joint_speed(self, id, speed):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_MOVING_SPEED, 3, id]))
        self.write(struct.pack('H', speed), wait=True)

    def write_joint_torque(self, id, torque):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_TORQUE_LIMIT, 3, id]))
        self.write(struct.pack('H', torque), wait=True)

    def write_tool_pos(self, x, z, speed):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_TOOL_POS, 6]))
        self.write(struct.pack('H', x))
        self.write(struct.pack('H', z))
        self.write(struct.pack('H', speed), wait=True)





