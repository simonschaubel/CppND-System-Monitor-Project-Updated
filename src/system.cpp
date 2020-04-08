#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "system.h"


void System::Update(){ linux_parser_.ParseAll(); }
std::vector<Process>& System::Processes() { return linux_parser_.Processes(); }
std::string System::Kernel() { return linux_parser_.Kernel(); }
float System::MemoryUtilization() { return linux_parser_.MemoryUtilization(); }
float System::CpuUtilization() { return linux_parser_.CpuUtilization(); }
std::string System::OperatingSystem() { return linux_parser_.OperatingSystem(); }
int System::RunningProcesses() { return linux_parser_.RunningProcesses(); }
int System::TotalProcesses() { return linux_parser_.TotalProcesses(); }
long int System::UpTime() { return linux_parser_.UpTime(); }