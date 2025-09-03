/* RasPike用ライブラリ */
#include "ForceSensor.h"

using namespace spikeapi;

class Starter {
public:
    Starter();
    void waitTouchForceSensor();

private:
    ForceSensor startButton;
};