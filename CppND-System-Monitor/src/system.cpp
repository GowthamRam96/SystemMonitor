#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>
//#include <functional>
#include "linux_parser.h"  // needed it for functions from linux_parser.cpp
#include "process.h"
#include "processor.h"
#include "system.h"
using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  vector<int> proc_pids = LinuxParser::Pids();
  for (int p : pids) {
    Process id_process(p);
    processes_.emplace_back(id_process);
  }
  std::sort(
      processes_.begin(), processes_.end(),
      std::less<
          Process>());  // https://en.cppreference.com/w/cpp/utility/functional/less
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() {
  return LinuxParser::OperatingSystem();
}  // string(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }