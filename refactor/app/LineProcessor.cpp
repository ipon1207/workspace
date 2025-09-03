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

void LineProcessor::handleLineColor() {
    array<double, 3> lineColorRate = getLineColor();

    if (lineColorRate[2] >= 0.48) { // 青色の割合を判定

        if (!isOnBlue) { // 青色を検知した瞬間
            incrementBlueCount();
            isOnBlue = true;
        }

    } else { // 青色を検知していない場合
        isOnBlue = false;
    }
}

int LineProcessor::getBlueCount() {
    return blueCount;
}

array<double, 3> LineProcessor::getLineColor() {
    lineSensor.getRGB(lineColorStructure);
    double all = lineColorStructure.r + lineColorStructure.g + lineColorStructure.b;
    double rRate = lineColorStructure.r / all;
    double gRate = lineColorStructure.g / all;
    double bRate = lineColorStructure.b / all;
    rgbRate[0] = rRate;
    rgbRate[1] = gRate;
    rgbRate[2] = bRate;
    return rgbRate;
}

void LineProcessor::incrementBlueCount() {
    blueCount++;
}