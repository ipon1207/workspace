/* RasPikeART用ライブラリ */
#include "Clock.h"
#include "ForceSensor.h"

using namespace spikeapi;

class Starter {
public:
    Starter();
    void waitStart();
    void printLog();

private:
    ForceSensor touchSensor;
    Clock waitTimer;
    const uint32_t waitTime = 100 * 1000;
};