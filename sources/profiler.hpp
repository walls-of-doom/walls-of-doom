#ifndef PROFILER_H
#define PROFILER_H

#include "clock.hpp"
#include "integers.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Profiler {
private:
  bool active;
  std::unordered_map<std::string, U64> events;
  std::unordered_map<std::string, TimePoint> started;
  std::unordered_map<std::string, double> maxima;
  std::unordered_map<std::string, double> minima;
  std::unordered_map<std::string, double> timings;
  std::vector<std::string> hierarchy;

  std::string get_component_name() const;
  TimePoint get_time_point() const;

public:
  explicit Profiler(bool active) : active(active) {
  }
  void start(const std::string &component);
  void stop();
  std::string dump();
};

#endif
