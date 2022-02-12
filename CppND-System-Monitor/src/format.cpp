#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hour = seconds / 3600;
  long minute = seconds / 60;
  long second = seconds; // % 60;
  std::string HH = std::to_string(hour);
  std::string MM = std::to_string(minute % 60); //ensure its does not display value > 60
  std::string SS = std::to_string(second % 60); //ensure its does not display value > 60
  std::string OUTPUT = HH + ":" + MM + ":" + SS;
  return OUTPUT;
}