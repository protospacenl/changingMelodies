#include "Stepper.h"

#define STEP_DELAY  200

void Stepper::step() 
{
    enable(true);

    digitalWrite(this->step_pin, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(this->step_pin, LOW);
    
    if (getDirection() == FORWARD) {
        this->current_steps++;
    } else {
        this->current_steps--;
    }
}


int Stepper::home(int dir)
{
    unsigned long travel_steps = 0;

    enable(true);

    setDirection(dir);
    while (digitalRead(endstop_pin) == ENDSTOP_INACTIVE) {
        step();
        travel_steps++;
        delayMicroseconds(300);

        if (travel_steps >= getMaxSteps()) {
           enable(false);
            return -1;
        }
    }

    setDirection(!dir);
    travel_steps = 0;
    while (digitalRead(endstop_pin) == ENDSTOP_ACTIVE) {
        step();
        delayMicroseconds(300);
        if (travel_steps >= getMaxSteps()) {
            enable(false);
            return -1;
        }
    }

    for (int i=5 * steps_per_mm; i>=0; i--) {
        step();
        delayMicroseconds(300);
        if (travel_steps >= getMaxSteps()) {
            enable(false);
            return -1;
        }
    }

    setDirection(dir);
    enable(false);
    resetSteps();

    return 0;
}

void Stepper::goTo(int mm)
{
    goto_step = mm * steps_per_mm;

    Serial.print("goto_step: ");
    Serial.print(goto_step);

    if (goto_step == current_steps)
        return;

    if (goto_step > current_steps) {
        setDirection(FORWARD);
    } else {
        setDirection(BACKWARD);
    }

    /*
    frame_delay = 1000000 / (steps_per_mm * mm);
    if (frame_delay < STEP_DELAY * 2) {
        frame_delay = STEP_DELAY;
    } else {
        frame_delay -= STEP_DELAY;
    }
    */

    enable(true);
    running = 1;
}

int Stepper::update()
{
    if (running) {
        if (digitalRead(endstop_pin) == ENDSTOP_ACTIVE) {
            running = 0;
            enable(false);
        } else {

            step();

            if (current_steps <= 0 || current_steps >= max_steps) {
                running = 0;
                enable(false);
            } else {
                if (current_direction == FORWARD) {
                    if (current_steps >= goto_step) {
                        running = 0;
                        enable(false);
                    }
                } else {
                    if (current_steps <= goto_step) {
                        running = 0;
                        enable(false);
                    }
                }
            }
        }
        return 1;
    }
    return 0;
}
