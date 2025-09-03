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
    int getBlueCount();
    void handleLineColor();

private:
    ColorSensor lineSensor;
    ColorSensor::RGB lineColorStructure;
    int blueCount = 0;
    array<double, 3> rgbRate = {0.0, 0.0, 0.0};

    array<double, 3> getLineColor();
    void incrementBlueCount();
};