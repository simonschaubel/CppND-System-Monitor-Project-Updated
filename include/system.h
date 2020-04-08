#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

class System {

 public:
  void Update();
  int TotalProcesses();
  int RunningProcesses();
  long UpTime();
  float CpuUtilization();
  float MemoryUtilization();
  std::string Kernel();
  std::string OperatingSystem();
  std::vector<Process>& Processes();

 private:
  LinuxParser linux_parser_;
};

#endif