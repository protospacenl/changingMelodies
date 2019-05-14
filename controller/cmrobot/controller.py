from serial import Serial
from cmrobot.singleton import Singleton

class Controller(metaclass=Singleton):
    def __init__(self, port="/dev/ttyUSB0", baudrate=19200, timeout=1, **kw):
        self.__serial = Serial(port=port, baudrate=baudrate, timeout=timeout, **kw)

    def write(self, data, wait=False):
        if not self.__serial:
            return None
        self.__serial.write(data)
