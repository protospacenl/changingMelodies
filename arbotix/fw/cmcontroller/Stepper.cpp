#include "Stepper.h"

void Stepper::step() 
{
    enable(true);

    digitalWrite(this->step_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(this->step_pin, LOW);
    
    if (this->current_direction == FORWARD) {
        this->current_steps++;
    } else {
        this->current_steps--;
    }
}


int Stepper::home(int dir)
{
    enable(true);

    resetSteps();

    direction(dir);
    while (digitalRead(endstop_pin) == ENDSTOP_INACTIVE) {
        step();
        delayMicroseconds(300);

        if (getSteps() >= getMaxSteps()) {
           enable(false);
            return -1;
        }
    }

    direction(!dir);
    while (digitalRead(endstop_pin) == ENDSTOP_ACTIVE) {
        step();
        delay(3);
        if (getSteps() >= getMaxSteps()) {
            enable(false);
            return -1;
        }
    }

    enable(false);
    resetSteps();

    return 0;
}
