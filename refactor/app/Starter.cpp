/* 宣言ファイル */
#include "Starter.h"

Starter::Starter() : startButton(EPort::PORT_D) {
}

void Starter::waitTouchForceSensor() {
    while (!startButton.isTouched()) {
    }
}