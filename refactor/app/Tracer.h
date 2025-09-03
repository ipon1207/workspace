/* RasPike用ライブラリ */
#include "Motor.h"

using namespace spikeapi;

class Tracer {
public:
    Tracer();
    void run(int reflection);
    void init();
    void terminate();

private:
    Motor leftWheel;
    Motor rightWheel;
    const int8_t mThreshold = 20;
    const int8_t pwm = 60;
    float calc_prop_value(int reflection);
};
