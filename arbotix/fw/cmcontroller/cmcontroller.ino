#include "ax12.h"
#include "PatrickController.h"
#include "Stepper.h"
#include "command.h"

#define TOOL_X_EN_PIN       7
#define TOOL_Z_EN_PIN       6
#define TOOL_X_STEP_PIN     5
#define TOOL_Z_STEP_PIN     4
#define TOOL_X_DIR_PIN      3
#define TOOL_Z_DIR_PIN      2
#define TOOL_X_ENDSTOP_PIN  16
#define TOOL_Z_ENDSTOP_PIN  17

#define MICROSTEP           4

#define POST_HOME_POSITION_X    120 /* mm */
#define POST_HOME_POSITION_Z    25

#define SERIAL_TIMEOUT      5000

#define STATE_IDLE  0
#define STATE_WAIT_START_2  1
#define STATE_WAIT_HEADER   2
#define STATE_WAIT_PAYLOAD  3

int current_state = 0;

command_t __command;
PatrickController __patrick = PatrickController(1000000); 
Stepper __spindle_x = Stepper(TOOL_X_EN_PIN, TOOL_X_STEP_PIN, TOOL_X_DIR_PIN, TOOL_X_ENDSTOP_PIN, MICROSTEP);
Stepper __spindle_z = Stepper(TOOL_Z_EN_PIN, TOOL_Z_STEP_PIN, TOOL_Z_DIR_PIN, TOOL_Z_ENDSTOP_PIN, MICROSTEP);

void tool_home(void)
{
    Serial.println("homing X\n");
    if ( __spindle_x.home(Stepper::BACKWARD, POST_HOME_POSITION_X) < 0) {
       Serial.println("Error homing X");
       Serial.write(CMD_RESP_ERR);
       for (;;) ;
    }
    Serial.println("X homed\n");
    
    Serial.println("homing Z\n");
    if ( __spindle_z.home(Stepper::BACKWARD, POST_HOME_POSITION_Z) < 0) {
       Serial.println("Error homing Y");
       Serial.write(CMD_RESP_ERR);
       for (;;) ;
    }
    Serial.println("Z homed\n");

    Serial.write(CMD_RESP_ACK);
}


int handle_command(command_t *msg, PatrickController *controller)
{
    int retval = -1;

    if (msg->cmd == CMD_ADD_SERVO) {
        cmd_add_servo_t * cmd = (cmd_add_servo_t*)&msg->params;
        retval = controller->addServo(cmd->id, cmd->type, cmd->P);
        if (retval > 0) {
            Serial.write(retval);
        } else {
            Serial.write(retval);
        }
    } else if (msg->cmd == CMD_TOOL_HOME) {
        tool_home();
    } else if (msg->cmd == CMD_SERVO_REPORT) {
        cmd_servo_report_t * cmd = (cmd_servo_report_t*)&msg->params;

        servo_report_t report;
        servo_t *servo = controller->getServo(cmd->id);
        controller->getServoReport(servo, &report);

        Serial.print(report.id);
        Serial.print(",");
        Serial.print(report.type);
        Serial.print(",");
        Serial.print(report.fw_version);
        Serial.print(",");
        Serial.print(report.position);
        Serial.print(",");
        Serial.print(report.voltage);
        Serial.print(",");
        Serial.print(report.temperature);
        Serial.print(",");
        Serial.print(report.load);
        Serial.print(",");
        Serial.print(report.error);
        Serial.println("");
        
    } else if (msg->cmd == CMD_MONITOR) {
        uint8_t idx;
        uint8_t n = controller->get_num_servos();

        for (idx=0; idx<n; idx++) {
            int pos;
            servo_t *s = controller->getServoByIdx(idx);
            if (s == NULL)
                continue;
            
            pos = dxlGetPosition(s->id);

            if (idx > 0) Serial.print(",");
            Serial.print(s->id);
            Serial.print(':'); 
            Serial.print(pos);
        }
        Serial.println("");

    } else if (msg->cmd == CMD_HOLD) {
        cmd_hold_t * cmd = (cmd_hold_t*)&msg->params;
        int pos1 = dxlGetPosition(cmd->id); 
        //int pos2 = dxlGetPosition(2); 
        //int pos3 = dxlGetPosition(3); 
        dxlTorqueOn(cmd->id);
        delay(3);
        controller->setServoPosition(cmd->id, pos1);
        /*
        dxlTorqueOn(2);
        delay(3);
        __patrick.setServoPosition(2, pos2);
        dxlTorqueOn(3);
        delay(3);
        __patrick.setServoPosition(3, pos3);
        */
    } else if (msg->cmd == CMD_TOOL_POSITION) {
        cmd_tool_position_t * cmd = (cmd_tool_position_t*)&msg->params;
        __spindle_z.goTo(cmd->z);
        __spindle_x.goTo(cmd->x);
        while (__spindle_z.isRunning() || __spindle_x.isRunning() ) { 
            __spindle_z.update();
            __spindle_x.update();
            delayMicroseconds(200);
        }
    } else if (msg->cmd == CMD_SERVO_GOAL_POSITION) {
        cmd_position_t * cmd = (cmd_position_t*)&msg->params;
        dxlTorqueOn(cmd->id);
        delay(3);
        __patrick.setServoPosition(cmd->id, cmd->position);
        //__patrick.initNextPosition(cmd->id, cmd->position);
    } else if (msg->cmd == CMD_SERVO_MOVING_SPEED) {
        cmd_speed_t * cmd = (cmd_speed_t*)&msg->params;
        __patrick.setServoSpeed(cmd->id, cmd->speed);
    } else if (msg->cmd == CMD_SERVO_TORQUE_LIMIT) {
        cmd_torque_t * cmd = (cmd_torque_t*)&msg->params;
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

    /*
    Serial.println("homing X\n");
    if ( __spindle_x.home(Stepper::BACKWARD, POST_HOME_POSITION_X) < 0) {
       Serial.println("Error homing X");
       Serial.write(CMD_RESP_ERR);
       for (;;) ;
    }
    Serial.println("X homed\n");
    
    Serial.println("homing Z\n");
    if ( __spindle_z.home(Stepper::BACKWARD, POST_HOME_POSITION_Z) < 0) {
       Serial.println("Error homing Y");
       Serial.write(CMD_RESP_ERR);
       for (;;) ;
    }
    Serial.println("Z homed\n");
    */

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
    //__spindle_x.update();
    //__spindle_z.update();
}
