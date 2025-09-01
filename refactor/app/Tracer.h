/* RasPikeART用ライブラリ */
#include "Clock.h"
#include "ColorSensor.h"
#include "Motor.h"

#include "app.h"

using namespace spikeapi;

//  障害物回避モードの変数をapp.cppから参照
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

private:
    Motor leftWheel;
    Motor rightWheel;
    const int8_t mThreshold = 20;
    const int8_t pwm = 50;
    float calc_prop_value();
};
