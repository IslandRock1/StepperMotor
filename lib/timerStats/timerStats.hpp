
#ifndef STEPPERMOTOR_TIMERSTATS_HPP
#define STEPPERMOTOR_TIMERSTATS_HPP

struct TimerData {
    long loopsPerSecond;
    double estimatedLoopTime;
};

class TimerStats {
public:
    TimerStats();

    void startTimer();
    void printTimerData() const;

    unsigned long long printIters = 0;
    int updateFreq = 10;

private:
    unsigned long prevTime;

    long loopsPerSecond = 0;
    double estimatedLoopTime = 0;
    unsigned long iters = 0;

    TimerData getTimerData();
};


#endif //STEPPERMOTOR_TIMERSTATS_HPP
