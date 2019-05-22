#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Stepper.h"

#define STEP_DELAY  200
#define delayMicroseconds(x) ;

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

void Stepper::goTo(uint16_t mm)
{
    goto_step = mm * steps_per_mm;

    printf("goto_step: %ld\n", goto_step);

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

#define TOOL_X_EN_PIN       7
#define TOOL_Z_EN_PIN       6
#define TOOL_X_STEP_PIN     5
#define TOOL_Z_STEP_PIN     4
#define TOOL_X_DIR_PIN      3
#define TOOL_Z_DIR_PIN      2
#define TOOL_X_ENDSTOP_PIN  16
#define TOOL_Z_ENDSTOP_PIN  17

#define MICROSTEP           4

Stepper __spindle_x = Stepper(TOOL_X_EN_PIN, TOOL_X_STEP_PIN, TOOL_X_DIR_PIN, TOOL_X_ENDSTOP_PIN, MICROSTEP);
Stepper __spindle_z = Stepper(TOOL_Z_EN_PIN, TOOL_Z_STEP_PIN, TOOL_Z_DIR_PIN, TOOL_Z_ENDSTOP_PIN, MICROSTEP);


int main(int argc, char *argv[])
{

    uint16_t mm_x = atoi(argv[1]);
    uint16_t mm_z = atoi(argv[2]);

    printf("Max steps: %ld\n", __spindle_x.getMaxSteps());
    printf("Go to: %d %d\n", mm_x, mm_z);
    fflush(stdout);

    __spindle_x.goTo(mm_x);
    __spindle_z.goTo(mm_z);
    while (__spindle_z.isRunning() || __spindle_x.isRunning() ) { 
        __spindle_z.update();
        __spindle_x.update();
        printf("Step x: %ld, z: %ld\n", __spindle_x.getSteps(), __spindle_z.getSteps());
    }

    return 0;
}

