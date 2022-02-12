#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long first_idleJiffies = 0;
  long first_ActJiffies = 0;
  long second_Idle_Jiffies = 0;
  long second_Act_Jiffies = 0;
};

#endif