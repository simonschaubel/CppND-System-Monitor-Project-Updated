#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

struct Processor
{
   Processor() : user(0), nice(0), system(0), idle(0), iowait(0),
                  irq(0), softirq(0), steal(0), guest(0), guestnice(0) {}
   Processor(const std::vector<int>& cpu) : 
    user(cpu[0]), nice(cpu[1]), system(cpu[2]), idle(cpu[3]), iowait(cpu[4]),
    irq(cpu[5]), softirq(cpu[6]), steal(cpu[7]), guest(cpu[8]), guestnice(cpu[9]) {}

   int user;
   int nice;
   int system;
   int idle;
   int iowait;
   int irq;
   int softirq;
   int steal;
   int guest;
   int guestnice;
};


#endif