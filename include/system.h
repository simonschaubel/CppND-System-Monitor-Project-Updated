#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

class System {
 public:
  void Update();
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float CpuUtilization();
  float MemoryUtilization();          // TODO: See src/system.cpp
  long UpTime();                      // TODO: See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // TODO: See src/system.cpp
  std::string OperatingSystem();      // TODO: See src/system.cpp

  // TODO: Define any necessary private members
 private:
  std::vector<Process> processes_ = {};
  LinuxParser linux_parser_;
};

#endif