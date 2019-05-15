#ifndef __STEPPER_H__
#define __STEPPER_H__

class Stepper {
    public:
        Stepper(uint8_t enable_pin, uint8_t step_pin, uint8_t dir_pin, uint8_t endstop_pin);

    private:
        unsigned long step;

        uint8_t enable_pin;
        uint8_t step_pin;
        uint8_t dir_pin;
        uint8_t endstop_pin;
}

#endif
