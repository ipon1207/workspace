/* 宣言ファイル */
#include "CalculationPID.h"

CalculationPID::CalculationPID() {
}

void CalculationPID::setReflection(int reflection) {
    this->reflection = reflection;
}

float CalculationPID::calclationPIDValue() {
    diffP = reflection - target;

    if (diffP * prevDiffP < 0) {
        integral = 0;
    }

    integral += diffP;

    if (targetD != -1) {
        diffD = reflection - targetD;
    }

    targetD = reflection;
    prevDiffP = diffP;
    calculatedValue = (Kp * diffP) + (Ki * integral) + (Kd * diffD);
    return calculatedValue;
}