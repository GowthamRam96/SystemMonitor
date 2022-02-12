#include "processor.h"

#include <unistd.h>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    
float cpu_utiliz = 0.0;
first_idleJiffies = LinuxParser::IdleJiffies();
first_ActJiffies = LinuxParser::ActiveJiffies();
sleep(1);  // difference in duration of reading can be adjusted later
second_Idle_Jiffies = LinuxParser::IdleJiffies();
second_Act_Jiffies = LinuxParser::ActiveJiffies();
long delta_IdleJiffies = second_Idle_Jiffies - first_idleJiffies;
long delta_ActJiffies = second_Act_Jiffies - first_ActJiffies;
cpu_utiliz = (float)(delta_ActJiffies) / (float)(delta_ActJiffies + delta_IdleJiffies);
return cpu_utiliz;

}