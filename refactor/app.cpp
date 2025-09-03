/* 標準ライブラリ */
#include <stdio.h>
/* RasPike用ライブラリ */
#include "Clock.h"
#include "ForceSensor.h"
/* 宣言ファイル*/
#include "app.h"
/* カスタムクラス */
#include "Starter.h"
#include "Tracer.h"

Tracer tracer;
Starter starter;
Clock clock;

using namespace spikeapi;

void tracer_task(intptr_t exinf) {
    tracer.run();
    ext_tsk();
}

void main_task(intptr_t unused) {
    const uint32_t duration = 100 * 1000;

    starter.waitTouchForceSensor();

    tracer.init();
    sta_cyc(TRACER_CYC);

    while (1) {
        clock.sleep(duration);
    }

    stp_cyc(TRACER_CYC);
    tracer.terminate();
    ext_tsk();
}
