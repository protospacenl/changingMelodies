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

#define MAX_SERVOS          3
#define SERVO_TYPE_AX       0
#define SERVO_TYPE_MX       1

#define AX_STEPSIZE        0.111328125f
#define AX_STEPSIZE_RECIP  8.982456f
#define AX_DEGR_PER_STEP   0.29f

/** a structure to hold transitions **/
typedef struct {
    uint8_t id;
    uint8_t type;
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

/** Patrick Controller Class for mega324p/644p clients. **/
class PatrickController
{
  public:
    /* For compatibility with legacy code */
    PatrickController(long baud);               // baud usually 1000000

    int addServo(uint8_t id, uint8_t type);
    servo_t * getServo(uint8_t id);
    void updateServos(void);
    void initNextPosition(uint8_t id, int position);

    void setServoSpeed(uint8_t id, int speed) {
        servo_t *servo = getServo(id);
        if (servo == NULL)
            return;
        servo->position_ms = speed;

        dxlSetGoalSpeed(id, speed);
        delay(3);
    };

    void setServoTorque(uint8_t id, uint16_t torque) {
        if (torque == 0) {
            dxlTorqueOff(id);
        } else {
            dxlTorqueOn(id);
            dxlSetRunningTorqueLimit(id, torque);
        }
        delay(3);
    };

    void setServoPosition(uint8_t id, uint16_t position, uint16_t speed=100, uint16_t torque=1023) {
        setServoTorque(id, torque);
        setServoSpeed(id, speed);
        dxlSetGoalPosition(id, position);
        delay(PATRICK_FRAME_DELAY);
    };


  private:  
    uint8_t nservos;
    servo_t servos[MAX_SERVOS];
  
};
#endif
