#include "Tracer.h"
#include <stdio.h>

Tracer::Tracer() : leftWheel(EPort::PORT_B, Motor::EDirection::COUNTERCLOCKWISE, true), rightWheel(EPort::PORT_A, Motor::EDirection::CLOCKWISE, true) {
}

void Tracer::terminate() {
    printf("Stopped.\n");
    leftWheel.stop();
    rightWheel.stop();
}

void Tracer::run(float turn) {
    int pwm_l = pwm - turn;
    int pwm_r = pwm + turn;
    leftWheel.setPower(pwm_l);
    rightWheel.setPower(pwm_r);
}