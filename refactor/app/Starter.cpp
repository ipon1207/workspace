/* 標準ライブラリ */
#include <stdio.h>
/* 宣言ファイル */
#include "Starter.h"

Starter::Starter() : touchSensor(EPort::PORT_D) {
    waitTimer.reset();
}

void Starter::waitStart() {
    while (!touchSensor.isTouched()) {
        waitTimer.sleep(waitTime);
        // printLog();
    }
}

void Starter::printLog() {
    printf("wait touchSensor\n");
}