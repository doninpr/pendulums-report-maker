#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using string = std::string;
using namespace std::chrono;

struct WORKLOG {
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
 
  milliseconds getDuration() {
    return stoppedAt - startedAt;
  }
 
  WORKLOG(json data) {
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

struct GROUP {
  milliseconds duration;
  std::vector<WORKLOG> works;

  void push_back(WORKLOG worklog) {
    works.push_back(worklog);
    duration += worklog.getDuration();
  }

  GROUP() {
    duration = milliseconds(0);
  }

  minutes getDurationInMinutes() {
    return duration_cast<minutes>(duration);
  }
};

int main(int argc, char** argv) {

  string path;

  if (argv[1] == NULL) {
    std::cout << "Enter path to JSON file: ";
    std::cin >> path;
  } else {
    path = argv[1];
  }

  json data;
  std::ifstream f(path);

  if (!f.is_open()) {
    std::cout << "Error: File not found" << std::endl;
    return 1;
  }

  try {
    data = json::parse(f);
  } catch (const std::exception& e) {
    std::cout << "Error: JSON not valid" << std::endl;
    return 1;
  }

  if (!data.is_array()) {
    std::cout << "Error: data is not an array" << std::endl;
    return 1;
  }

  std::map<string, GROUP> works;

  for (json::iterator it = data.begin(); it != data.end(); ++it) {
    WORKLOG worklog = WORKLOG(*it);
    works[worklog.name].push_back(worklog);
  }

  for (auto it = works.begin(); it != works.end(); ++it) {
    std::cout << it->first << ": " << it->second.getDurationInMinutes().count() << std::endl;
  }
  
  return 0;
}