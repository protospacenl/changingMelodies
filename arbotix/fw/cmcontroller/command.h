#ifndef __COMMAND_H__
#define __COMMAND_H__

uint8_t CMD_HEADER[2] = {0xff, 0xff};

#define CMD_RESP_ACK              0xff
#define CMD_RESP_ERR              0xee
#define CMD_ADD_SERVO             0x01
#define CMD_SERVO_GOAL_POSITION   0x1E
#define CMD_SERVO_MOVING_SPEED    0x20
#define CMD_SERVO_TORQUE_LIMIT    0x22

typedef struct __attribute__(( packed)) cmd_add_servo_t {
    uint8_t id;
    uint8_t type;
} cmd_add_servo_t;

typedef struct __attribute__(( packed)) cmd_speed_t {
    uint8_t id;
    uint16_t speed;
} cmd_speed_t;

typedef struct __attribute__(( packed)) cmd_torque_t {
    uint8_t id;
    uint16_t torque;
} cmd_torque_t;

typedef struct __attribute__(( packed)) cmd_position_t {
    uint8_t id;
    uint16_t position;
} cmd_position_t;

typedef struct __attribute__(( packed)) command_t {
    uint8_t cmd;
    uint8_t size;
    union {
        cmd_speed_t moving_speed;
        cmd_torque_t torque_limit;
        cmd_position_t goal_positon;
    } params;
} command_t;


#endif
