/* 宣言ファイル */
#include "ColorController.h"

ColorController::ColorController() : lineColorSensor(EPort::PORT_E) {
    lineColor.r = 0;
    lineColor.g = 0;
    lineColor.b = 0;
}

void ColorController::incrementBlueCount() {
    blueCount++;
}

int ColorController::getLineReflection() {
    reflection = lineColorSensor.getReflection();
    return reflection;
}

int ColorController::getBlueCount() {
    return blueCount;
}

array<double, 3> ColorController::getLineColorRate() {
    lineColorSensor.getRGB(lineColor);
    array<double, 3> rgb = {lineColor.r, lineColor.g, lineColor.b};
    return rgb;
}