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
        delay(3);
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

void Stepper::goTo(int mm, int mms)
{
    goto_step = mm * steps_per_mm;

    if (goto_step == current_steps)
        return;

    if (goto_step > current_steps) {
        setDirection(FORWARD);
    } else {
        setDirection(BACKWARD);
    }

    frame_delay = 1000000 / (steps_per_mm * mm);
    if (frame_delay < STEP_DELAY * 2) {
        frame_delay = STEP_DELAY;
    } else {
        frame_delay -= STEP_DELAY;
    }

    enable(true);
    running = 1;
    last_frame = micros();
}

void Stepper::update()
{
    if (running) {
        if ((micros() - last_frame) > frame_delay) {
            last_frame = micros();

            if (digitalRead(endstop_pin) == ENDSTOP_ACTIVE) {
                running = 0;
                enable(false);
            } else {

                step();

                if (current_steps <= 0 || current_steps >= max_steps || current_steps == goto_step) {

                    running = 0;
                    enable(false);
                }
            }
        }
    }
}
