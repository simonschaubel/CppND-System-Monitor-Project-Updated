#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;


// Setter
void Process::setCmd(std::string cmd){ this->cmd_ = cmd;}
void Process::setUser(std::string user){ this->user_ = user;}
void Process::setPid(int pid){ this->pid_ = pid;}
void Process::setClockTicks( long int clock_ticks){this->clock_ticks_ = clock_ticks;}
void Process::setSystemUpTimePrev( long int uptime){this->system_uptime_prev_ = uptime;}
void Process::setRam( long int ram){this->ram_ = ram;}
void Process::setTotalCpuTicks( float ticks){this->totalCpuTicks_ = ticks;}
void Process::setTotalCpuTicksPrev( float ticks){this->totalCpuTicksPrev_ = ticks;}

// Getter
int Process::Pid() { return pid_; }

float Process::CpuUtilization() { 
    float deltaCpuTime = (totalCpuTicks_- totalCpuTicksPrev_) / sysconf(_SC_CLK_TCK);
    float deltaUpTime = UpTime() - system_uptime_prev_;
    return deltaCpuTime / deltaUpTime;
}

float Process::CpuTicks() { return totalCpuTicks_; }
long int Process::ClockTicks() { return clock_ticks_; }

string Process::Command() { return cmd_; }

unsigned int Process::Ram() { return ram_ / 1000; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_ticks_- clock_ticks_ / sysconf(_SC_CLK_TCK); }

bool Process::operator<(Process const& b) const { return b.ram_ < ram_ ? true : false; }