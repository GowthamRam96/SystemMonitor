#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTotal, memFree, value;  
  string key;
  string line;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);  // /proc/meminfo
  if (filestream.is_open()) {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');  // remove ':' character
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if(key == "MemTotal")
        {
          memTotal = value;
        }
        if(key == "MemFree")
        {
          memFree = value;
          break;
        }
      }
    }  
  
  }
  return (float)(memTotal - memFree) / (float)memTotal; // usage = (MemTotal - MemFree) / MemTotal
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime_string_tmp = "0";  // ensure everything is zero
  long uptime = 0;  // ensure everything is zero at the beggining
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_string_tmp;
    
  }
  //uptime = std::atoi(uptime_string_tmp.c_str());  // Convert string to int, maybe need long
  uptime = std::stol(uptime_string_tmp);
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies_sum;
  vector<string> jiffies = LinuxParser::CpuUtilization();  // test
  for (auto i :
       jiffies)  // iterate through the cpuN for(string each_jiffie : jiffies)
  {
    jiffies_sum += std::stoi(i);  // add all 10 and convert
  }

  return jiffies_sum;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// https://www.generacodice.com/en/articolo/104476/how-to-calculate-the-cpu-usage-of-a-process-by-pid-in-linux-from-c
long LinuxParser::ActiveJiffies(
    int pid)  // https://man7.org/linux/man-pages/man5/proc.5.html
{
  string line;
  string key;
  string value;
  vector<string> pid_stat = {};
  string pid_num = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pid_num + kStatFilename);  //
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 17; i++) {
      linestream >> value;
      pid_stat.emplace_back(value);  // utime, stime, cutime, cstime starting at
                                  // array[13] to array[17]
    }
  }
  long total_active_Jiffies = std::stoi(pid_stat[13]) +
                              std::stoi(pid_stat[14]) +
                              std::stoi(pid_stat[15]) + std::stoi(pid_stat[16]);

  return total_active_Jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long jiffies_active = 0;
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long j_user = std::stoi(jiffies[CPUStates::kUser_]);
  long j_nice = std::stoi(jiffies[CPUStates::kNice_]);
  long j_steal = std::stoi(jiffies[CPUStates::kSteal_]);
  long j_sirq = std::stoi(jiffies[CPUStates::kSoftIRQ_]);
  long j_irq = std::stoi(jiffies[CPUStates::kIRQ_]);
  long j_sys = std::stoi(jiffies[CPUStates::kSystem_]);
  jiffies_active = j_user + j_nice + j_steal + j_sirq + j_irq + j_sys;
  return jiffies_active;
}
// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_iowait = 0;
  vector<string> jiffies = LinuxParser::CpuUtilization();  // test
  long idle = std::stoi(jiffies[CPUStates::kIdle_]);
  long iowait = std::stoi(jiffies[CPUStates::kIOwait_]);
  idle_iowait = idle + iowait;

  return idle_iowait;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  string values;
  vector<string> cpuN;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line))  
    {
      std::istringstream linestream(line);
      linestream >> key;  
      if (key == "cpu") {
        while (linestream >> values)  
        {                             
          cpuN.push_back(values);     
        }
      }
    }
  }
  return cpuN;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes;  // return type
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        processes =
            stoi(value);   // convert back to int, since return type is int
        return processes;  // test
      }
    }
  }
  return processes;
  // return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int numberOfProcess;
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        numberOfProcess = stoi(value);
        return numberOfProcess;
      }
    }
  }
  return numberOfProcess;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  //
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    if(line == "")
    {
      return "None";
    }
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  string ramValue;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');  
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmRSS") {
        linestream >> value;
        ramValue = to_string((stoi(value))/1024);  // convert value to int, convert KB to MB
        return ramValue;
        
      }
    }
  }
  return ramValue;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  string pid_num = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pid_num + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid") {
        linestream >> value;
        return value;
        break;
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function

string LinuxParser::User(int pid) {
  string line;
  string user_key;
  string xCharacter;
  string user_passwd;
  string user_id = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':',
                   ' ');  // replace : so next characters can be push to value
      std::istringstream linestream(line);
      linestream >> user_key >> xCharacter >> user_passwd;
      if (user_passwd == user_id) {
        return user_key;
        break;
      }
    }
  }
  return user_key;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string key;
  string value;
  string pid_num = std::to_string(pid);

  std::ifstream filestream(kProcDirectory + pid_num + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream streamline(line);
    for (int i = 0; i < 22; i++) {
      streamline >> value;  
    }
    long system_uptime = LinuxParser::UpTime();
    long uptime_process =
        system_uptime - (std::stoi(value) /
                         sysconf(_SC_CLK_TCK)); 
    return uptime_process;
  }
  return 0;
}
