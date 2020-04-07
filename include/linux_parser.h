#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

#include "processor.h"

class LinuxParser{
  public:
   void ParsesAll();
   int TotalProcesses();
   int RunningProcesses();
   long UpTime();
   float CpuUtilization();
   float MemoryUtilization();
   std::string Kernel();
   std::string OperatingSystem();
   std::vector<int> Pids();

  private:
   // Methods and Functions
   void ParseOsRelease();
   void ParseProcVersion();
   void ParseProcMeminfo();
   void ParseProcStat();
   void ParseUpTime();

   // Variables
   int totalProcesses;
   int runningProcesses;
   long upTime;
   float totalMemory;
   float freeMemory; 
   std::string operatingSystem;
   std::string linuxKernel;
   Processor processor;
   Processor prev_processor = Processor();

   // Paths
   const std::string kProcDirectory{"/proc/"};
   const std::string kCmdlineFilename{"/cmdline"};
   const std::string kCpuinfoFilename{"/cpuinfo"};
   const std::string kStatusFilename{"/status"};
   const std::string kStatFilename{"/stat"};
   const std::string kUptimeFilename{"/uptime"};
   const std::string kMeminfoFilename{"/meminfo"};
   const std::string kVersionFilename{"/version"};
   const std::string kOSPath{"/etc/os-release"};
   const std::string kPasswordPath{"/etc/passwd"};

   long Jiffies();
   long ActiveJiffies();
   long ActiveJiffies(int pid);
   long IdleJiffies();
   
   // Processes
   std::string Command(int pid);
   std::string Ram(int pid);
   std::string Uid(int pid);
   std::string User(int pid);
   long int UpTime(int pid);
};

#endif