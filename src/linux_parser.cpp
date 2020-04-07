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
void LinuxParser::ParsesAll(){
  ParseOsRelease();
  ParseProcVersion();
  ParseProcMeminfo();
  ParseProcStat();
  ParseUpTime();
}

int LinuxParser::RunningProcesses() { return totalProcesses; }

int LinuxParser::TotalProcesses(){ return runningProcesses; }

long LinuxParser::UpTime() { return upTime; }

float LinuxParser::CpuUtilization() {
  // Calculate idle and non ide ov previous processor and current
  int prev_idle = prev_processor.idle + prev_processor.iowait;
  int idle = processor.idle +processor.iowait;

  int prev_nonIdle = prev_processor.user + prev_processor.nice + prev_processor.system
                    + prev_processor.irq + prev_processor.softirq + prev_processor.steal;
  int nonIdle = processor.user + processor.nice + processor.system
                + processor.irq + processor.softirq + processor.steal;
  // write back current to previous 
  prev_processor = processor;
  
  int prev_total = prev_idle + prev_nonIdle;
  int total = idle + nonIdle;

  int delta_total = total - prev_total;
  int delta_idle = idle - prev_idle;

  return float(delta_total - delta_idle) / delta_total;
}

float LinuxParser::MemoryUtilization() { return (totalMemory - freeMemory) / totalMemory; }

string LinuxParser::OperatingSystem() { return operatingSystem; }

string LinuxParser::Kernel() { return linuxKernel; }

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
          operatingSystem = value;
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
    linuxKernel = kernel;
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
        totalMemory = std::stof(value);
      }
      else if (name == "MemFree:"){
        freeMemory = std::stof(value);
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
        totalProcesses = std::stoi(line_vector[1]);
      }
      else if (line_vector[0] == "procs_running"){
        runningProcesses = std::stoi(line_vector[1]);
      }
      else if (line_vector[0] == "cpu"){
        // Transform string vector to int vector
        std::vector<int> cpu_stats;
        std::transform(line_vector.begin() + 1, line_vector.end(), std::back_inserter(cpu_stats),
                       [](const std::string& str) { return std::stoi(str); });
        processor = Processor(cpu_stats);
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
    upTime = std::stol(up_time);
  }
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}