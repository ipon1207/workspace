/* 標準ライブラリ */
#include <array>
/* RasPikeART用ライブラリ */
#include "ColorSensor.h"

using namespace std;
using namespace spikeapi;

class ColorController {
public:
    ColorController();
    void incrementBlueCount();
    int getLineReflection();
    int getBlueCount();
    array<double, 3> getLineColorRate();

private:
    int reflection = 0;
    int blueCount = 0;
    ColorSensor lineColorSensor;
    ColorSensor::RGB lineColor;
};