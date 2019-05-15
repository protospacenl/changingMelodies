#include <Arduino.h>

#ifndef __STEPPER_H__
#define __STEPPER_H__

class Stepper {
    public:
        static const int FORWARD = LOW;
        static const int BACKWARD = HIGH;
        static const int UP = LOW;
        static const int DOWN = HIGH;

        static const int ENDSTOP_ACTIVE   = LOW;
        static const int ENDSTOP_INACTIVE = HIGH;

        Stepper(uint8_t enable_pin, uint8_t step_pin, uint8_t dir_pin, uint8_t endstop_pin, int microstep) {
            this->enable_pin = enable_pin;
            this->step_pin = step_pin;
            this->dir_pin = dir_pin;
            this->endstop_pin = endstop_pin;

            pinMode(this->enable_pin, OUTPUT);
            pinMode(this->step_pin, OUTPUT);
            pinMode(this->dir_pin, OUTPUT);
            pinMode(this->endstop_pin, INPUT);

            this->current_steps = 0;
            this->current_direction = FORWARD;
            this->microstep = microstep;
            
            this->steps_per_mm = (this->microstep * 200) / 5;
            this->max_steps = this->steps_per_mm * 300;
        };

        uint8_t direction() { return current_direction; };
        void direction(uint8_t direction) { 
            digitalWrite(this->dir_pin, direction);
            current_direction = direction;
        };

        void enable(bool enable) {
            if (enable)
                digitalWrite(this->enable_pin, HIGH);
            else
                digitalWrite(this->enable_pin, LOW);
        };

        int home(int dir);

        void resetSteps() { current_steps = 0; };
        unsigned long getSteps() { return current_steps; };
        unsigned long getMaxSteps() { return max_steps; };

        void step();
        void step(int dir) {
            direction(dir);
            step();
        };

    private:
        unsigned long current_steps;
        unsigned long max_steps;
        uint8_t current_direction;
        int microstep;
        int steps_per_mm;

        uint8_t enable_pin;
        uint8_t step_pin;
        uint8_t dir_pin;
        uint8_t endstop_pin;
};

#endif
