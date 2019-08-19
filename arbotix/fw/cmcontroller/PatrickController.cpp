/*
  PatrickController.cpp - Based on ArbotiX Library for Patrick Pose Engine
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

#include "PatrickController.h"
#include <ax12.h>

/* initializes serial1 transmit at baud, 8-N-1 */
PatrickController::PatrickController(long baud)
{
    memset(servos, 0x00, MAX_SERVOS * sizeof(servo_t));
    ax12Init(baud);  
    nservos = 0;
}

int PatrickController::addServo(uint8_t id, uint8_t type, uint8_t P)
{
    int i;
    uint16_t position;

    if (nservos >= MAX_SERVOS)
        return -1;

    for (i=0; i<nservos; i++) {
        if (id == servos[i].id)
            return -1;
    }

    servos[nservos].id = id;
    servos[nservos].type = type;
    servos[nservos].running = 0;

    if (type == SERVO_TYPE_MX) {
      mxSetP(id, P);
    }
    
    dxlLEDOn(id);
    delay(100);
    dxlLEDOff(id);
    
    nservos++;

    return nservos;
}

servo_t * PatrickController::getServo(uint8_t id)
{
    for (int i=0; i<nservos; i++) {
        if (servos[i].id == id) {
            return &servos[i];
        }
    }
    return NULL;
}

void PatrickController::updateServos(void)
{
    int s;

    for (s=0; s<nservos; s++) {
        servo_t *servo = &servos[s]; 

        if (millis() - servo->last_frame < PATRICK_FRAME_DELAY)
            continue;

        servo->last_frame = millis();

        if (servo->running) {
            int diff = servo->next_position - servo->current_position_set;
            if (diff == 0) {
                servo->running = 0;
            } else {
                if (diff > 0) {
                    if (diff < servo->speed) {
                        servo->current_position_set = servo->next_position;
                        servo->running = 0;
                    } else {
                        servo->current_position_set += servo->speed;
                    }
                } else {
                    if ((-diff) < servo->speed) {
                        servo->current_position_set = servo->next_position;
                        servo->running = 0;
                    } else {
                        servo->current_position_set -= servo->speed;
                    }
                }
            }

            dxlSetGoalPosition(servo->id, (servo->current_position_set >> POSITION_SHIFT));
        } else {
        }
    }
}

void PatrickController::initNextPosition(uint8_t id, int position) 
{
    servo_t *servo;
    int frames = 0;

    servo = getServo(id);
    if (servo == NULL)
        return;
    
    int curpos = dxlGetPosition(id);

    frames = (servo->position_ms / PATRICK_FRAME_DELAY) + 1;
    servo->current_position_set = curpos << POSITION_SHIFT;
    servo->next_position = position << POSITION_SHIFT;
    servo->last_frame = millis();

    if(servo->next_position > servo->current_position_set) {
        servo->speed = (servo->next_position - servo->current_position_set) / frames + 1;
    }else{
        servo->speed = (servo->current_position_set - servo->next_position) / frames + 1;
    }

    servo->running = 1;
}
