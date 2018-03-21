#include "profiler.hpp"
#include "integers.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>

struct Record {
  std::string base;
  std::string name;
  std::string full;
  double time;

  Record(std::string full_name, double time) : full(full_name), time(time) {
    auto separator = full_name.find_last_of('.');
    if (separator != std::string::npos) {
      base = full_name.substr(0, separator);
      name = full_name.substr(separator + 1);
    } else {
      name = full_name;
    }
  }

  bool operator<(const Record &other) { return base < other.base || (base == other.base && time > other.time); }
};

static double seconds_between(const TimePoint now, const TimePoint then) {
  const std::chrono::duration<double> delta = now - then;
  return delta.count();
}

TimePoint Profiler::get_time_point() const { return std::chrono::steady_clock::now(); }

std::string Profiler::get_component_name() const {
  std::string name;
  bool first = true;
  for (const auto &bit : hierarchy) {
    if (!first) {
      name += '.';
    }
    name += bit;
    first = false;
  }
  return name;
}

void Profiler::start(const std::string &component) {
  if (!active) {
    return;
  }
  hierarchy.push_back(component);
  const std::string &component_name = get_component_name();
  events[component_name]++;
  started[component_name] = get_time_point();
}

void Profiler::stop() {
  if (!active) {
    return;
  }
  const std::string &component_name = get_component_name();
  double duration = seconds_between(get_time_point(), started[component_name]);
  maxima[component_name] = std::max(maxima[component_name], duration);
  minima[component_name] = std::min(minima[component_name], duration);
  timings[component_name] += duration;
  hierarchy.pop_back();
}

static std::string double_to_string(double value, int places) {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(places) << value;
  return stream.str();
}

static std::string seconds_to_milliseconds_string(double value) { return double_to_string(1000.0 * value, 2) + " ms"; }

std::string Profiler::dump() {
  if (!active) {
    return "";
  }
  std::vector<Record> records;
  std::unordered_map<std::string, double> base_total;
  for (auto timing : timings) {
    const Record &record = Record(timing.first, timing.second);
    base_total[record.base] += record.time;
    records.push_back(record);
  }
  std::sort(records.begin(), records.end());
  std::stringstream stream;
  const std::vector<std::string> labels = {"Event", "Min", "Max", "Mean", "Count", "Ratio"};
  for (size_t i = 0; i < labels.size(); i++) {
    if (i > 0) {
      stream << ',';
    }
    stream << labels[i];
  }
  stream << '\n';
  auto dump_record = [this, &base_total, &stream](const Record &record) {
    stream << record.full << ',';
    stream << seconds_to_milliseconds_string(minima[record.full]) << ',';
    stream << seconds_to_milliseconds_string(maxima[record.full]) << ',';
    stream << seconds_to_milliseconds_string(record.time / events[record.full]) << ',';
    stream << std::to_string(events[record.full]) << ',';
    stream << double_to_string(record.time / base_total[record.base], 2);
  };
  // After dumping a record, dump all of its children.
  std::function<void(size_t)> dump_tree = [&records, &stream, &dump_record, &dump_tree](size_t i) {
    dump_record(records[i]);
    stream << '\n';
    for (size_t j = 0; j < records.size(); j++) {
      if (records[j].base == records[i].full) {
        dump_tree(j);
      }
    }
  };
  for (size_t i = 0; i < records.size(); i++) {
    if (records[i].base.empty()) {
      dump_tree(i);
    }
  }
  return stream.str();
}
