import struct
from serial import Serial
from cmrobot.singleton import Singleton

class Controller(metaclass=Singleton):
    CMD_ACK                   = b'\xff'

    CMD_HEADER                = 0xffff
    CMD_ADD_SERVO             = 0x01
    CMD_TOOL_POSITION         = 0x02
    CMD_SERVO_MONITOR         = 0x03
    CMD_SERVO_HOLD            = 0x04
    CMD_TOOL_HOME             = 0x05
    CMD_SERVO_REPORT          = 0x06
    CMD_SERVO_GOAL_POSITION   = 0x1E
    CMD_SERVO_MOVING_SPEED    = 0x20
    CMD_SERVO_TORQUE_LIMIT    = 0x22

    ERR_DICT = [ {'name': 'VOLTAGE', 'mask': 0x01 },
                 {'name': 'ANGLE_LIMIT', 'mask': 0x04 },
                 {'name': 'OVERHEATING', 'mask': 0x08 },
                 {'name': 'CHECKSUM', 'mask': 0x16 },
                 {'name': 'OVERLOAD', 'mask': 0x32 },
                 {'name': 'INSTRUCTION', 'mask': 0x64 }]


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
            #print(f"got ack: {ack!r}")
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

    def add_servo(self, id, type, P):
        cmd = struct.pack('HBBBBB', self.CMD_HEADER, 
                                    self.CMD_ADD_SERVO, 
                                    3, id, type, P)
        print(f"Adding servo: {cmd!r}")
        self.write(cmd, wait=True)

    def hold(self, id):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_HOLD, 1, id]), wait=True)

    def tool_home(self):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_TOOL_HOME, 1, 0]), wait=True)

    def monitor(self):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_MONITOR, 1, 0]))
        s = b''
        while True:
            c = self.__serial.read(1)
            if c == b'':
                continue
            if c == self.CMD_ACK:
                break
            s += c
        #print(f"{s}")

        v = s.decode().split(',')
        positions = [ [ int(x[0]), int(x[1]) ] for x in [ v.split(':') for v in v ] ]

        return positions

    def report(self, name, id):
        self.write(struct.pack('H', self.CMD_HEADER))
        self.write(bytes([self.CMD_SERVO_REPORT, 1, id]))
        s = b''
        while True:
            c = self.__serial.read(1)
            if c == b'':
                continue
            if c == self.CMD_ACK:
                break
            s += c

        print(s)
        v = s.decode().split(',')

        if len(v) <= 1 or len(v) > 8:
            return False

        load = int(v[6]) - 1024
        #current = 0.0045 * current

        errors = ''
        error_byte = int(v[7])
        for e in self.ERR_DICT:
            if error_byte & e['mask']:
                errors = errors + " " + e['name']
        if errors == '':
            errors = 'NONE'

        print(f"{name} -> ID:{v[0]}, Type:{v[1]}, Fwv: {v[2]}, Pos: {v[3]}, Voltage: {int(v[4])/10}, Temperature: {v[5]}, Load: {load}, Error: {errors}")
        return True


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
        self.write(bytes([self.CMD_TOOL_POSITION, 6]))
        self.write(struct.pack('H', x))
        self.write(struct.pack('H', z))
        self.write(struct.pack('H', speed), wait=True)





