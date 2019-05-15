#include "ax12.h"
#include "PatrickController.h"
//#include <BioloidController.h>
#include "command.h"

#define SERIAL_TIMEOUT      5000

#define STATE_IDLE  0
#define STATE_WAIT_START_2  1
#define STATE_WAIT_HEADER   2
#define STATE_WAIT_PAYLOAD  3

int current_state = 0;

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
        dxlTorqueOn(cmd->id);
        delay(3);
        __patrick.setServoPosition(cmd->id, cmd->position);
        //__patrick.initNextPosition(cmd->id, cmd->position);
    } else if (msg->cmd == CMD_SERVO_MOVING_SPEED) {
        cmd_speed_t * cmd = (cmd_speed_t*)&msg->params;
        Serial.print(cmd->speed);
        __patrick.setServoSpeed(cmd->id, cmd->speed);
    } else if (msg->cmd == CMD_SERVO_TORQUE_LIMIT) {
        cmd_torque_t * cmd = (cmd_torque_t*)&msg->params;
        Serial.print(cmd->torque);
        if (cmd->torque == 0) {
            dxlTorqueOff(cmd->id);
        } else {
            dxlTorqueOn(cmd->id);
            __patrick.setServoTorque(cmd->id, cmd->torque);
        }
    } else {
        ;
    }
}


void setup(void)
{
    Serial.begin(19200);
    Serial.setTimeout(SERIAL_TIMEOUT);
    delay(500);
    /* send ack */
    Serial.write(CMD_RESP_ACK);

}

void loop(void)
{
    while(Serial.available() > 0) {
        if(current_state == STATE_IDLE) {
            if(Serial.read() == 0xff) {
                current_state = STATE_WAIT_START_2;
            }
        } else if(current_state == STATE_WAIT_START_2) { 
            if(Serial.read() == 0xff)
                current_state = STATE_WAIT_HEADER;
            else
                current_state = STATE_IDLE;
        } else if(current_state == STATE_WAIT_HEADER) {
            if (Serial.available() >= 2) {
                Serial.readBytes((char*)&__command, 2);    
                current_state = STATE_WAIT_PAYLOAD;
            }
        } else if(current_state == STATE_WAIT_PAYLOAD) {
            if (Serial.available() >= __command.size) {
                int retval = -1;
                Serial.readBytes((char*)&__command.params, __command.size);
                
                retval = handle_command(&__command, &__patrick);
                if (retval < 0) {
                    Serial.write(CMD_RESP_ERR);
                } else {
                    Serial.write(CMD_RESP_ACK);
                }
                current_state = STATE_IDLE;
            }
        } 
    }

    __patrick.updateServos();
}
