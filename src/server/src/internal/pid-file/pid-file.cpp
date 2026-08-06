#include <csignal>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include "pid-file.hpp"
#include "vicinae.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

bool PidFile::kill() {
  if (auto n = pid()) {
#ifdef _WIN32
    HANDLE const handle = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(*n));
    if (!handle) return false;
    bool const ok = TerminateProcess(handle, 1) != 0;
    CloseHandle(handle);
    return ok;
#else
    return ::kill(*n, SIGINT) == 0;
#endif
  }

  return false;
}

std::optional<int> PidFile::pid() {
  std::ifstream ifs(path());

  if (!ifs) return std::nullopt;

  std::string line;
  std::getline(ifs, line);
  std::istringstream iss(line);
  int pid;

  iss >> pid;

  return pid;
}

bool PidFile::exists() { return fs::exists(path()); }

void PidFile::write(int pid) { std::ofstream(path()) << std::to_string(pid); }

fs::path PidFile::path() const { return Omnicast::runtimeDir() / std::format("{}.pid", m_name); }

PidFile::PidFile(const std::string &name) : m_name(name) {}
