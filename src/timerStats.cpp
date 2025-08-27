
#include <Arduino.h>
#include "timerStats.hpp"

TimerStats::TimerStats() {
    prevTime = millis();
}

void TimerStats::startTimer() {
    iters++;
    printIters++;

    if (iters % updateFreq == 0) {
        estimatedLoopTime = static_cast<double>(millis() - prevTime) / static_cast<double>(updateFreq);
        loopsPerSecond = static_cast<long>(1000.0 / estimatedLoopTime);
        iters = 0;
        prevTime = millis();

        updateFreq = loopsPerSecond;
        printTimerData();
    }
}

void TimerStats::printTimerData() const {
    Serial.print("Loops per second: ");
    Serial.print(loopsPerSecond);
    Serial.print(" | ");
    Serial.print("Estimated loop-time: ");
    Serial.print(estimatedLoopTime * 1000);
    Serial.println(" us.");
}

TimerData TimerStats::getTimerData() {
    return {loopsPerSecond, estimatedLoopTime};
}