#include "ColorSensor.h"
#include "Motor.h"
#include "app.h"

#include "Clock.h"

using namespace spikeapi;

extern volatile int avoid_mode;
extern Clock avoidance_timer;
extern volatile int straight_mode; // 直進モード

class Tracer {
public:
    Tracer();
    void run();
    void init();
    void terminate();
    ColorSensor colorSensor;
    Motor leftWheel;
    Motor rightWheel;

private:
    const int8_t pwm = 45;
    float calc_prop_value();
};
