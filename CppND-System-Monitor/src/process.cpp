#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
  cpu_ = CpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  float cpu_utilize = 0.0;
  float syc_clk_tck = sysconf(_SC_CLK_TCK);
  long upTime = LinuxParser::UpTime();  // test
  long upTimeProcess = LinuxParser::UpTime(pid_);
  long totalJiffies = LinuxParser::ActiveJiffies(pid_);
  float duration_sec = upTime - (upTimeProcess / syc_clk_tck);
  cpu_utilize = (float)totalJiffies / duration_sec/ syc_clk_tck; 

  return cpu_utilize;

}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return cpu_ < a.cpu_;

}
