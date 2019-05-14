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
    uint16_t new_position;
    uint16_t speed;
    uint16_t torque;
    uint16_t temperature;
    uint16_t state;
} servo_t; 

/** Patrick Controller Class for mega324p/644p clients. **/
class PatrickController
{
  public:
    /* For compatibility with legacy code */
    PatrickController(long baud);               // baud usually 1000000

    int addServo(uint8_t id, uint8_t type);

    void setServoSpeed(uint8_t id, uint16_t speed) {
        dxlSetGoalSpeed(id, speed);
        delay(PATRICK_FRAME_DELAY);
    };

    void setServoTorque(uint8_t id, uint16_t torque) {
        dxlSetRunningTorqueLimit(id, torque);
        delay(PATRICK_FRAME_DELAY);
    };

    void setServoPosition(uint8_t id, uint16_t position) {
        dxlSetGoalPosition(id, position);
        delay(PATRICK_FRAME_DELAY);
    };

    void setServoPosition(uint8_t id, uint16_t position, uint16_t speed) {
        setServoSpeed(id, speed);
        setServoPosition(id, position);
    };

    void setServoPosition(uint8_t id, uint16_t position, uint16_t speed, uint16_t torque) {
        setServoTorque(id, torque);
        setServoPosition(id, position, speed);
    };

  private:  
    uint8_t nservos;
    servo_t servos[MAX_SERVOS];
  
};
#endif
