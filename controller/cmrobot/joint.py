from cmrobot.controller import Controller

class Joint():
    MAX_MOVING_SPEED    = 0x3FF
    MAX_TORQUE_LIMIT    = 0x3FF
    SERVO_TYPE_AX       = 0x00
    SERVO_TYPE_MX       = 0x01

    def __init__(self, name="noname", type="AX", type_id=0, id=0, params={}, controller=None):
        self.__name = name
        self.__type = type
        self.__type_id = type_id
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

    @property
    def type_id(self):
        return self.__type_id

    @property
    def params(self):
        return self.__params


    def set_goal_position(self, position):
        self.__controller.write_joint_position(self.id, position)

    def set_moving_speed(self, speed):
        self.__controller.write_joint_speed(self.id, speed)

    def set_torque_limit(self, torque):
        if torque == -1:
            torque = self.MAX_TORQUE_LIMIT
        self.__controller.write_joint_torque(self.id, torque)

    def move_to(self, position, speed=0, torque=0):
        print(f"moving {self.name} to {position} with speed: {speed} and torque {torque}")
        self.set_moving_speed(speed)
        self.set_torque_limit(torque)
        self.set_goal_position(position)
        
    def relax(self):
        print(f"{self.name} relaxing")
        self.set_torque_limit(0)

    def hold(self):
        print(f"{self.name} holding")
        if 'holding_torque' in self.params:
            self.set_torque_limit(self.params['holding_torque'])
            if self.params['holding_torque'] > 0:
                self.__controller.hold(self.id)
        else:
            self.__controller.hold(self.id)

class JointMX(Joint):
    MAX_MOVING_SPEED    = 0x3FF
    MAX_TORQUE_LIMIT    = 0xfff

    def __init__(self, **kw):
        super(JointMX, self).__init__(type_id=self.SERVO_TYPE_MX, **kw)
        

    def move_to(self, position, speed=0, torque=0):
        #print("MX move")
        super(JointMX, self).move_to(position, speed, torque)

    def relax(self):
        #print("MX relax")
        super(JointMX, self).relax()

class JointAX(Joint):
    def __init__(self, **kw):
        super(JointAX, self).__init__(type_id=self.SERVO_TYPE_AX, **kw)

    def move_to(self, position, speed=0, torque=0):
        #print("AX move")
        super(JointAX, self).move_to(position, speed, torque)

    def relax(self):
        #print("AX relax")
        super(JointAX, self).relax()


