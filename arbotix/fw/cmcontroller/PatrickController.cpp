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

int PatrickController::addServo(uint8_t id, uint8_t type)
{
    int i;

    if (nservos >= MAX_SERVOS)
        return -1;

    for (i=0; i<nservos; i++) {
        if (id == servos[i].id)
            return -1;
    }

    servos[nservos].id = id;
    servos[nservos].type = type;

    nservos++;

    return nservos;
}


