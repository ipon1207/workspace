#include "ColorSensor.h" 
#include "Motor.h"   
#include "app.h"

#include "Clock.h"


using namespace spikeapi;

//ここから8/4記述　心配
// 障害物回避モードの変数をapp.cppから参照
extern volatile int avoid_mode;
extern Clock avoidance_timer;
//ここまで

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
#ifndef MAKE_RASPIKE
  const int8_t pwm = 30;
#else
  const int8_t pwm = 50;
#endif
  float calc_prop_value();
};
