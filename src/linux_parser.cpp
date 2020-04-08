#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


// LinuxParser Interfaces
void LinuxParser::ParseAll(){
  ParseOsRelease();
  ParseProcVersion();
  ParseProcMeminfo();
  ParseProcStat();
  ParseUpTime();
  ParseUid();
  ParsePids();
  ParseProcesses();   
}

int LinuxParser::RunningProcesses() { return totalProcesses_; }

int LinuxParser::TotalProcesses(){ return runningProcesses_; }

long LinuxParser::UpTime() { return upTime_; }

float LinuxParser::CpuUtilization() {
  // Calculate idle and non ide ov previous processor and current
  int prev_idle = prev_processor_.idle + prev_processor_.iowait;
  int idle = processor_.idle +processor_.iowait;

  int prev_nonIdle = prev_processor_.user + prev_processor_.nice + prev_processor_.system
                    + prev_processor_.irq + prev_processor_.softirq + prev_processor_.steal;
  int nonIdle = processor_.user + processor_.nice + processor_.system
                + processor_.irq + processor_.softirq + processor_.steal;
  // write back current to previous 
  prev_processor_ = processor_;
  
  int prev_total = prev_idle + prev_nonIdle;
  int total = idle + nonIdle;

  int delta_total = total - prev_total;
  int delta_idle = idle - prev_idle;

  return float(delta_total - delta_idle) / delta_total;
}

float LinuxParser::MemoryUtilization() { return (totalMemory_ - freeMemory_) / totalMemory_; }

string LinuxParser::OperatingSystem() { return operatingSystem_; }

string LinuxParser::Kernel() { return linuxKernel_; }

std::vector<Process>& LinuxParser::Processes() { 
  sort(processes_.begin(), processes_.end());
  return processes_; 
}

// LinuxParser Functions
void LinuxParser::ParseOsRelease(){
  string line, key, value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          operatingSystem_ = value;
        }
      }
    }
  }
}

void LinuxParser::ParseProcVersion(){
  string os, version, kernel, line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    linuxKernel_ = kernel;
  }
}

void LinuxParser::ParseProcMeminfo(){
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string line;
    string name, value, unit;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> name >> value >> unit;
      if (name == "MemTotal:"){
        totalMemory_ = std::stof(value);
      }
      if (name == "MemFree:"){
        freeMemory_ = std::stof(value);
      }
    }
  }
}

void LinuxParser::ParseProcStat(){
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      std::istream_iterator<std::string> begin(linestream), end;
      std::vector<std::string> line_vector(begin, end);
      
      if (line_vector[0] == "processes"){
        totalProcesses_ = std::stoi(line_vector[1]);
      }
      if (line_vector[0] == "procs_running"){
        runningProcesses_ = std::stoi(line_vector[1]);
      }
      if (line_vector[0] == "cpu"){
        // Transform string vector to int vector
        std::vector<int> cpu_stats;
        std::transform(line_vector.begin() + 1, line_vector.end(), std::back_inserter(cpu_stats),
                       [](const std::string& str) { return std::stoi(str); });
        processor_ = Processor(cpu_stats);
      }
    }
  }
}

void LinuxParser::ParseUpTime(){
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string up_time, idle_time;
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
    upTime_ = std::stol(up_time);
  }
}

void LinuxParser::ParseUid(){
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line;
    while(std::getline(filestream, line)){
      std::string element;
      std::vector<std::string> linevector;
      std::istringstream linestream(line);
      
      // Extract line to vector and create hashmap with uid and names
      while(std::getline(linestream, element, ':')){ linevector.push_back(element); }
      uids_.insert(std::make_pair(std::stoi(linevector[2]), linevector[0]));
    }
  }
}

// BONUS: Update this to use std::filesystem
void LinuxParser::ParsePids() {
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  pids_.clear();
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        pids_.push_back(std::stoi(filename));
      }
    }
  }
  closedir(directory);
}

void LinuxParser::ParseProcesses(){
  prev_processes_ = processes_;
  processes_.clear();
  for(auto pid : pids_){
    Process process(pid, upTime_);
    ParsePStatus(process);
    ParsePStat(process);
    ParsePCmdLine(process);
    for (auto prev_process : prev_processes_){
      if (prev_process.Pid() == pid){
        process.setTotalCpuTicksPrev(prev_process.CpuTicks());
        process.setSystemUpTimePrev(prev_process.UpTime());
      }
    }
    processes_.push_back(process);
  }
}

void LinuxParser::ParsePStatus(Process& process){
  std::ifstream filestream(kProcDirectory + std::to_string(process.Pid()) + kStatusFilename);
  if (filestream.is_open()) {
    string line;
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      std::istream_iterator<std::string> begin(linestream), end;
      std::vector<std::string> line_vector(begin, end);
      
      if (line_vector[0] == "Uid:"){
        process.setUser(uids_[std::stoi(line_vector[1])]);
      }
      if (line_vector[0] == "VmSize:"){
        process.setRam(std::stol(line_vector[1]));
      }
    }
  }  
}

void LinuxParser::ParsePStat(Process& process){
  std::ifstream filestream(kProcDirectory + std::to_string(process.Pid()) + kStatFilename);
  if (filestream.is_open()) {
    std::istream_iterator<std::string> begin(filestream), end;
    std::vector<std::string> line_vector(begin, end);
    process.setClockTicks(std::stol(line_vector[ProcessStatus::pstarttime]));
    process.setTotalCpuTicks(  std::stol(line_vector[ProcessStatus::putime]) 
                             + std::stol(line_vector[ProcessStatus::pstime])
                             + std::stol(line_vector[ProcessStatus::pcutime])
                             + std::stol(line_vector[ProcessStatus::pcstime]));
  }
}

void LinuxParser::ParsePCmdLine(Process& process){
  std::ifstream filestream(kProcDirectory + std::to_string(process.Pid()) + kCmdlineFilename);
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    process.setCmd(line);
  }
}
