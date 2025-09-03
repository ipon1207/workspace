/* RasPike用ライブラリ */
#include "Motor.h"

using namespace spikeapi;

class Tracer {
public:
    Tracer();
    void run(float turn);
    void terminate();

private:
    Motor leftWheel;
    Motor rightWheel;
    const int8_t pwm = 60;
};
