#pragma once
#include <chrono>
#include <memory>

class GlobalTimer {
  static std::unique_ptr<GlobalTimer> uniqueGlobalTimer;
  std::chrono::seconds timeLimit;
  std::chrono::high_resolution_clock::time_point startTime;

  GlobalTimer() {}
  void restart(std::chrono::seconds seconds) {
    timeLimit = seconds;
    startTime = std::chrono::high_resolution_clock::now();
  }

public:
  static void initialTimerAndSetTimeLimit(std::chrono::seconds seconds) {
    if (uniqueGlobalTimer == nullptr)
      uniqueGlobalTimer = std::unique_ptr<GlobalTimer>(new GlobalTimer);
    uniqueGlobalTimer->restart(seconds);
  }
  static GlobalTimer *getInstance() { return uniqueGlobalTimer.get(); }
  template <class ToDuration = std::chrono::nanoseconds>
  ToDuration getDuration() {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<ToDuration>(endTime - startTime);
  }
  bool overTime() { return getDuration<>() >= timeLimit; }
};