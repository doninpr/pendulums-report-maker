#include <algorithm>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using string = std::string;
using namespace std::chrono;

namespace reportsMaker {
struct date {
  int day;
  int month;
  int year;

  date(milliseconds timestamp) {
    time_t time = system_clock::to_time_t(system_clock::time_point(timestamp));
    tm *localTime = localtime(&time);
    day = localTime->tm_mday;
    month = localTime->tm_mon + 1;
    year = localTime->tm_year + 1900;
  }
};

struct worklog {
  milliseconds createdAt;
  string id;
  string name;
  string project;
  milliseconds startedAt;
  milliseconds stoppedAt;
  milliseconds updatedAt;
  string user;

  milliseconds getTimestamp(json data) {
    long timestamp = 0;
    if (data.is_string()) {
      timestamp = std::stol(data.get<string>());
    } else if (data.is_number()) {
      timestamp = data.get<int>();
    }
    return milliseconds(timestamp);
  }

  date getDate() { return date(startedAt); }

  string getDateString() {
    date worklogDate = getDate();
    return std::to_string(worklogDate.day) + "." +
           std::to_string(worklogDate.month) + "." +
           std::to_string(worklogDate.year);
  }

  int getMonth() {
    time_t time = system_clock::to_time_t(system_clock::time_point(startedAt));
    tm *localTime = localtime(&time);
    return localTime->tm_mon + 1;
  }

  int getYear() {
    time_t time = system_clock::to_time_t(system_clock::time_point(startedAt));
    tm *localTime = localtime(&time);
    return localTime->tm_year + 1900;
  }

  milliseconds getDuration() { return stoppedAt - startedAt; }

  worklog(json data) {
    createdAt = getTimestamp(data["createdAt"]);
    id = data["id"];
    name = data["name"];
    project = data["project"];
    startedAt = getTimestamp(data["startedAt"]);
    stoppedAt = getTimestamp(data["stoppedAt"]);
    updatedAt = getTimestamp(data["updatedAt"]);
    user = data["user"];
  }
};

struct group {
  milliseconds duration;
  std::vector<string> works;

  void push_back(worklog worklog) {
    if (std::find(works.begin(), works.end(), worklog.name) == works.end()) {
      works.push_back(worklog.name);
    }
    duration += worklog.getDuration();
  }

  group() { duration = milliseconds(0); }

  minutes getDurationInMinutes() { return duration_cast<minutes>(duration); }
};
} // namespace reportsMaker