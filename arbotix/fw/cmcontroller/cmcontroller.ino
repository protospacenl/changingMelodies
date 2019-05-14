#include "ax12.h"
#include "PatrickController.h"

#define SERIAL_TIMEOUT            5000

uint8_t CMD_HEADER[2]           = {0xff, 0xff};
#define CMD_RESP_ACK              0xff
#define CMD_RESP_ERR              0xee
#define CMD_ADD_SERVO             0x01
#define CMD_SERVO_GOAL_POSITION   0x1E
#define CMD_SERVO_MOVING_SPEED    0x20
#define CMD_SERVO_TORQUE_LIMIT    0x22

int mode = 0;

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

command_t __command;
PatrickController __patrick = PatrickController(1000000); 

int handle_command(command_t *msg, PatrickController *controller)
{
    int retval = -1;

    if (msg->cmd == CMD_ADD_SERVO) {
        cmd_add_servo_t * cmd = (cmd_add_servo_t*)&msg->params;
        retval = controller->addServo(cmd->id, cmd->type);
        if (retval > 0) {
            Serial.write(retval);
        } else {
            Serial.write(retval);
        }
    } else if (msg->cmd == CMD_SERVO_GOAL_POSITION) {
        cmd_position_t * cmd = (cmd_position_t*)&msg->params;
        Serial.write(cmd->id);
        Serial.print(cmd->position);
    } else if (msg->cmd == CMD_SERVO_MOVING_SPEED) {
        cmd_speed_t * cmd = (cmd_speed_t*)&msg->params;
    } else if (msg->cmd == CMD_SERVO_TORQUE_LIMIT) {
        cmd_torque_t * cmd = (cmd_torque_t*)&msg->params;
    } else {
        ;
    }
}


void setup(void)
{
    Serial.begin(19200);
    Serial.setTimeout(SERIAL_TIMEOUT);
    delay(500);
    Serial.write(CMD_RESP_ACK);
}

void loop(void)
{

    while(Serial.available() > 0) {
        
        if(mode == 0) {
            if(Serial.read() == 0xff) {
                mode = 1;
            }
        } else if(mode == 1) { 
            if(Serial.read() == 0xff)
                mode = 2;
            else
                mode = 0;
        } else if(mode == 2) {   // next byte is index of servo
            if (Serial.available() >= 2) {
                Serial.readBytes((char*)&__command, 2);    
                mode = 3;
            }
        } else if(mode == 3) {   // next byte is length
            if (Serial.available() >= __command.size) {
                int retval = -1;
                Serial.readBytes((char*)&__command.params, __command.size);
                
                retval = handle_command(&__command, &__patrick);
                if (retval < 0) {
                    Serial.write(CMD_RESP_ERR);
                } else {
                    Serial.write(CMD_RESP_ACK);
                }

                mode = 0;
            }
        } 
    }
}
