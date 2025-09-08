#ifdef __cplusplus
extern "C" {
#endif

#include "spikeapi.h"

#define MAIN_PRIORITY (TMIN_APP_TPRI + 1)
#define TRACER_PRIORITY (TMIN_APP_TPRI + 2)

#ifndef STACK_SIZE
#define STACK_SIZE (4096)
#endif /* STACK_SIZE */

#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);
extern void tracer_cyc(intptr_t exinf);
extern volatile int count_blue;
extern volatile int mode_lr;
extern volatile int target_D;    // D制御用の変数、微分のための前回の値を保持する変数
extern volatile int integral;    // I制御の変数
extern volatile int prev_diff_P; // I制御の変数のリセットを判断するための変数
extern volatile bool turn_const;
extern volatile bool is_black; // 回避モード中、黒色を検知したかどうかを示すもの

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif