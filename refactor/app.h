#include "spikeapi.h"

#define MAIN_PRIORITY (TMIN_APP_TPRI + 1)
#define TRACER_PRIORITY (TMIN_APP_TPRI + 2)

#ifndef STACK_SIZE
#define STACK_SIZE (4096)
#endif /* STACK_SIZE */

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);
extern void tracer_cyc(intptr_t exinf);