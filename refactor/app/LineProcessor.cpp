/* 宣言ファイル */
#include "LineProcessor.h"

LineProcessor::LineProcessor() : lineSensor(EPort::PORT_E) {
    lineColorStructure.r = 0;
    lineColorStructure.g = 0;
    lineColorStructure.b = 0;
}

int32_t LineProcessor::getLineReflection() {
    return lineSensor.getReflection();
}

array<double, 3> LineProcessor::getLineColor() {
    lineSensor.getRGB(lineColorStructure);
    double all = lineColorStructure.r + lineColorStructure.g + lineColorStructure.b;
    double r_rate = lineColorStructure.r / all;
    double g_rate = lineColorStructure.g / all;
    double b_rate = lineColorStructure.b / all;
    rgb_rate[0] = r_rate;
    rgb_rate[1] = g_rate;
    rgb_rate[2] = b_rate;
    return rgb_rate;
}

int LineProcessor::getBlueCount() {
    return blueCount;
}

void LineProcessor::incrementBlueCount() {
    blueCount++;
}