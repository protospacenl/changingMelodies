#include "Stepper.h"

Stepper::Stepper(uint8_t enable_pin, uint8_t step_pin, uint8_t dir_pin, uint8_t endstop_pin)
{
    this.enable_pin = enable_pin;
    this.step_pin = step_pin;
    this.dir_pin = dir_pin;
    this.endstop_pin = endstop_pin;

    pinMode(this.enable_pin, OUTPUT);
    pinMode(this.step_pin, OUTPUT);
    pinMode(this.dir_pin, OUTPUT);
    pinmode(this.endstop_pin, INPUT);

    this.step = 0;
}
