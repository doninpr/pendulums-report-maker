#include "main.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <math.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using string = std::string;
using namespace std::chrono;
using namespace reportsMaker;

float roundFloat(float number) { return std::round(number * 100) / 100; }

string getStringFromDuration(float duration) {
  string durationStr = std::to_string(duration);

  for (std::string::reverse_iterator i = durationStr.rbegin();
       i != durationStr.rend(); ++i) {
    if (*i == '0') {
      durationStr.erase(i.base() - 1);
    } else if (*i == '.') {
      durationStr.erase(i.base() - 1);
      break;
    } else {
      break;
    }
  }

  return durationStr;
}

int main(int argc, char **argv) {

  string path;

  if (argv[1] == NULL) {
    std::cout << "Enter path to JSON file: ";
    std::cin >> path;
  } else {
    path = argv[1];
  }

  int monthNumber;
  std::cout << "Enter month number [1-12]: ";
  std::cin >> monthNumber;

  if (monthNumber < 1 || monthNumber > 12) {
    std::cout << "Error: month number must be between 1 and 12" << std::endl;
    return 1;
  }

  int yearNumber;
  std::cout << "Enter a year [4 digits]: ";
  std::cin >> yearNumber;

  json data;
  std::ifstream f(path);

  if (!f.is_open()) {
    std::cout << "Error: File not found" << std::endl;
    return 1;
  }

  try {
    data = json::parse(f);
  } catch (const std::exception &e) {
    std::cout << "Error: JSON not valid" << std::endl;
    return 1;
  }

  if (!data.is_array()) {
    std::cout << "Error: data is not an array" << std::endl;
    return 1;
  }

  // creating works map
  std::map<int, group> works;
  for (json::iterator it = data.begin(); it != data.end(); ++it) {
    worklog worklogInstance = worklog(*it);
    date worklogDate = worklogInstance.getDate();
    if (worklogDate.month != monthNumber || worklogDate.year != yearNumber) {
      continue;
    }
    works[worklogDate.day].push_back(worklogInstance);
  }

  string csvSource =
      "\"Дата\";Наименование услуг;\"Количество рабочих часов\"\n";

  float totalDuration = 0;

  for (auto it = works.begin(); it != works.end(); ++it) {
    float duration =
        roundFloat(it->second.getDurationInMinutes().count() / 60.0);
    totalDuration += duration;

    string worksString = "";
    for (auto workName : it->second.works) {
      worksString += workName + ",\n";
    }

    string dateString = std::to_string(it->first) + "." +
                        std::to_string(monthNumber) + "." +
                        std::to_string(yearNumber);

    string durationStr = getStringFromDuration(duration);

    csvSource += "\"" + dateString + "\";\"" + worksString + "\";\"" +
                 durationStr + "\"\n";
  }

  csvSource +=
      "\"Итого\";\"\";\"" + std::to_string(roundFloat(totalDuration)) + "\"\n";

  std::ofstream outfile("pendulumus-export-" + std::to_string(monthNumber) +
                        "-" + std::to_string(yearNumber) + ".csv");
  outfile << csvSource << std::endl;
  outfile.close();

  return 0;
}