/*
  PatrickController.h - Based on ArbotiX Library for Bioloid Pose Engine
  Copyright (c) 2008-2012 Michael E. Ferguson.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PatrickController_h
#define PatrickController_h

#include "ax12.h"

#define PATRICK_FRAME_DELAY 33
#define POSITION_SHIFT      3

#define MAX_SERVOS          16
#define SERVO_TYPE_AX       0
#define SERVO_TYPE_MX       1

#define AX_STEPSIZE        0.111328125f
#define AX_STEPSIZE_RECIP  8.982456f
#define AX_DEGR_PER_STEP   0.29f

/** a structure to hold transitions **/
typedef struct {
    uint8_t id;
    uint8_t type;
    uint8_t P;
    int current_position;
    int current_position_set;
    int next_position;
    int temperature;
    int torque_limit;
    int torque_enabled;
    int speed;
    int position_ms;
    int state;
    int running;
    unsigned long last_frame;
} servo_t; 

typedef struct {
    uint8_t id;
    int position; 
    int voltage;
    int temperature;
    int load;
    uint8_t type;
    int fw_version;
    int error;
    uint8_t P;
} servo_report_t;

/** Patrick Controller Class for mega324p/644p clients. **/
class PatrickController
{
  public:
    /* For compatibility with legacy code */
    PatrickController(long baud);               // baud usually 1000000

    int addServo(uint8_t id, uint8_t type, uint8_t P);
    servo_t * getServo(uint8_t id);
    void updateServos(void);
    void initNextPosition(uint8_t id, int position);

    uint8_t get_num_servos(void) { return nservos; };
    servo_t * getServoByIdx(uint8_t idx) {
        if (idx >= nservos)
            return NULL;
        return &servos[idx];
    }

    void setServoSpeed(uint8_t id, int speed) {
        servo_t *servo = getServo(id);
        if (servo == NULL)
            return;
        servo->position_ms = speed;

        dxlSetGoalSpeed(id, speed);
        delay(33);
    };

    void setServoTorque(uint8_t id, uint16_t torque) {
        if (torque == 0) {
            dxlTorqueOff(id);
        } else {
            dxlTorqueOn(id);
            delay(3);
            dxlSetRunningTorqueLimit(id, torque);
        }
        delay(33);
    };

    void setServoPosition(uint8_t id, uint16_t position) {
        dxlSetGoalPosition(id, position);
        delay(PATRICK_FRAME_DELAY);
    };

    uint8_t getServoReport(servo_t *servo, servo_report_t *report) {
        if (servo == NULL || report == NULL) {
            report->id = 255;
            return -1;
        }
      
        uint8_t id = servo->id;
      
        report->id = id;
        report->position = dxlGetPosition(id); 
        report->voltage = dxlGetVoltage(id);
        report->temperature = dxlGetTemperature(id);
        report->type = servo->type;
        report->fw_version = dxlGetFirmwareVersion(id);
        report->error = dxlGetError(id);
        report->P = servo->P;
        report->load = dxlGetTorque(id);

        return 0;
    };

  private:  
    uint8_t nservos;
    servo_t servos[MAX_SERVOS];
  
};
#endif
