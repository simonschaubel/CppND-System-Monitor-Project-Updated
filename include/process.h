#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  
  // Constructors
  Process() : pid_(0), ram_(0), clock_ticks_(0), uptime_ticks_(0), system_uptime_prev_(0), 
              totalCpuTicks_(0), totalCpuTicksPrev_(0), cmd_(std::string()), user_(std::string()){}
  Process(int pid, long int uptime_ticks) : 
              pid_(pid), ram_(0), clock_ticks_(0), uptime_ticks_(uptime_ticks), system_uptime_prev_(0), 
              totalCpuTicks_(0), totalCpuTicksPrev_(0), cmd_(std::string()), user_(std::string()){}

  // Setter
  void setCmd(std::string cmd);
  void setUser(std::string user);
  void setPid(int pid);
  void setClockTicks(long int clock_ticks);
  void setSystemUpTimePrev(long int uptime);
  void setRam(long int ram);
  void setTotalCpuTicks(float ticks);
  void setTotalCpuTicksPrev(float ticks);
  
  // Getter
  int Pid();
  unsigned int Ram();
  long int ClockTicks();
  long int UpTime();
  float CpuTicks();
  float CpuUtilization();
  std::string User();
  std::string Command();

  bool operator<(Process const& b) const;

 private:
  int pid_;
  long int ram_;
  long int clock_ticks_;
  long int uptime_ticks_;
  float system_uptime_prev_;
  float totalCpuTicks_;
  float totalCpuTicksPrev_;
  std::string cmd_;
  std::string user_;
};

#endif