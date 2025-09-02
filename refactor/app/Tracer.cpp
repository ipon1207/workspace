#include "Tracer.h"
#include <stdio.h>

Tracer::Tracer() : leftWheel(EPort::PORT_B, Motor::EDirection::COUNTERCLOCKWISE, true), rightWheel(EPort::PORT_A, Motor::EDirection::CLOCKWISE, true), colorSensor(EPort::PORT_E) {
}

void Tracer::init() {
    printf("Tracer\n");
}

void Tracer::terminate() {
    printf("Stopped.\n");
    leftWheel.stop();
    rightWheel.stop();
}

void Tracer::run() {
    float turn = calc_prop_value();
    int pwm_l = pwm - turn;
    int pwm_r = pwm + turn;
    leftWheel.setPower(pwm_l);
    rightWheel.setPower(pwm_r);
}

float Tracer::calc_prop_value() {

    const float Kp = 0.83;
    const int target = 10;
    const int bias = 0;

    int diff = colorSensor.getReflection() - target;
    return (Kp * diff + bias);
}