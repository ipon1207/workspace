/* RasPike用ライブラリ */
#include "ColorSensor.h"
#include "Motor.h"

using namespace spikeapi;

class Tracer {
public:
    Tracer();
    void run();
    void init();
    void terminate();

private:
    Motor leftWheel;
    Motor rightWheel;
    ColorSensor colorSensor;
    const int8_t mThreshold = 20;
    const int8_t pwm = 60;
    float calc_prop_value();
};
