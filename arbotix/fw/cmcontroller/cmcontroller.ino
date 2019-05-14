#include "ax12.h"
#include "PatrickController.h"

#define SERIAL_TIMEOUT      5000
uint8_t CMD_HEADER[2] = {0xff, 0xff};

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

void setup(void)
{
    Serial.begin(19200);
    Serial.setTimeout(SERIAL_TIMEOUT);
    delay(500);
    Serial.write(0xff);
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
                Serial.readBytes((char*)&__command.params, __command.size);
                if (__command.cmd == 0x01) {
                    int retval;
                    cmd_add_servo_t *cmd = (cmd_add_servo_t*)&__command.params;
                    retval = __patrick.addServo(cmd->id, cmd->type);
                    if (retval < 0) {
                        Serial.write(0xee);
                    } else {
                        Serial.write((uint8_t)retval);
                        Serial.write(0xff);
                    }
                } else {
                    Serial.write(0xff);
                }
                mode = 0;
            }
        } 
    }
}
