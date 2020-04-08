#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

#include "processor.h"
#include "process.h"

class LinuxParser{
  public:
   void ParseAll();
   int TotalProcesses();
   int RunningProcesses();
   long UpTime();
   float CpuUtilization();
   float MemoryUtilization();
   std::string Kernel();
   std::string OperatingSystem();
   std::vector<Process>& Processes();
   
  private:
   // Methods and Functions
   void ParseOsRelease();
   void ParseProcVersion();
   void ParseProcMeminfo();
   void ParseProcStat();
   void ParseUpTime();
   void ParseUid();
   void ParsePids();
   void ParseProcesses();
   void ParsePStatus(Process& process);
   void ParsePStat(Process& process);
   void ParsePCmdLine(Process& process);

   // Variables
   int totalProcesses_;
   int runningProcesses_;
   long upTime_;
   float totalMemory_;
   float freeMemory_; 
   std::string operatingSystem_;
   std::string linuxKernel_;
   Processor processor_;
   Processor prev_processor_ = Processor();
   std::map<int, std::string> uids_;
   std::vector<Process> processes_;
   std::vector<Process> prev_processes_;
   std::vector<int> pids_;

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

enum ProcessStatus{ putime = 14, pstime = 15, pcutime = 16, pcstime = 17, pstarttime = 21 };

#endif