/* 標準ライブラリ */
#include <stdio.h>
/* RasPike用ライブラリ */
#include "Clock.h"
#include "ForceSensor.h"
/* 宣言ファイル*/
#include "app.h"
/* カスタムクラス */
#include "LineProcessor.h"
#include "Starter.h"
#include "Tracer.h"

Tracer tracer;
Starter starter;
Clock clock;
LineProcessor lineProcessor;

using namespace spikeapi;

void tracer_task(intptr_t exinf) {
    lineProcessor.handleLineColor();
    printf("blue count is %d\n", lineProcessor.getBlueCount());
    tracer.run(lineProcessor.getLineReflection());
    ext_tsk();
}

void main_task(intptr_t unused) {
    const uint32_t duration = 100 * 1000;

    starter.waitTouchForceSensor();

    tracer.init();
    sta_cyc(TRACER_CYC);

    while (1) {
        clock.sleep(duration);
        if (lineProcessor.getBlueCount() >= 6) {
            break;
        }
    }

    stp_cyc(TRACER_CYC);

    clock.sleep(duration); // 最後の周期タスクの終了を待つ
    tracer.terminate();

    ext_tsk();
}
