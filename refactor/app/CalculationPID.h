enum class TraceMode {
    LEFT,
    RIGHT,
};

class CalculationPID {
public:
    CalculationPID();
    void setReflection(int reflection);
    float calclationPIDValue();

private:
    const int target = 55;
    const float Kp = 0.55;
    const float Ki = 0.0001;
    const float Kd = 0.20;
    const TraceMode traceMode = TraceMode::LEFT;
    int diffP = 0;
    int prevDiffP = 0;
    int integral = 0;
    int diffD = 0;
    int targetD = 0;
    int reflection = 0;
    float calculatedValue = 0.0;
};