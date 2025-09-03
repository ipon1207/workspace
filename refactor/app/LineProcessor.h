/* 標準ライブラリ */
#include <array>
/* RasPike用ライブラリ */
#include "ColorSensor.h"

using namespace std;
using namespace spikeapi;

class LineProcessor {
public:
    LineProcessor();
    int32_t getLineReflection();
    array<double, 3> getLineColor();
    int getBlueCount();
    void incrementBlueCount();

private:
    ColorSensor lineSensor;
    ColorSensor::RGB lineColorStructure;
    int blueCount = 0;
    array<double, 3> rgb_rate = {0.0, 0.0, 0.0};
};