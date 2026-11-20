#include <csignal>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <iterator>
#include <sstream>
#include <string>
#include "pid-file.hpp"
#include "vicinae.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

namespace fs = std::filesystem;

#ifndef _WIN32
namespace {
// The pid file outlives the process it refers to, and pids get recycled. Before signalling a pid
// read from disk we make sure it still belongs to the program that wrote the file, otherwise we
// could SIGINT an entirely unrelated process (a build, a package manager, ...).
bool isProcessRelatedTo(int pid, const std::string &name) {
  std::ifstream ifs(fs::path{"/proc"} / std::to_string(pid) / "cmdline", std::ios::binary);

  if (!ifs) return false;

  std::string cmdline((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  // the file is a sequence of NUL separated arguments; comparing against the basename only would
  // not work for helpers started through an interpreter (the extension manager is a node process)
  return cmdline.find(name) != std::string::npos;
}
} // namespace
#endif

bool PidFile::kill() {
  if (auto n = pid()) {
#ifdef _WIN32
    HANDLE const handle = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(*n));
    if (!handle) return false;
    bool const ok = TerminateProcess(handle, 1) != 0;
    CloseHandle(handle);
    return ok;
#else
    if (*n == ::getpid()) return false;
    if (!isProcessRelatedTo(*n, m_name)) return false;
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
  int pid = 0;

  // a pid of 0 or below would target the caller's own process group, or every process
  if (!(iss >> pid) || pid <= 0) return std::nullopt;

  return pid;
}

bool PidFile::exists() { return fs::exists(path()); }

void PidFile::write(int pid) { std::ofstream(path()) << std::to_string(pid); }

fs::path PidFile::path() const { return Omnicast::runtimeDir() / std::format("{}.pid", m_name); }

PidFile::PidFile(const std::string &name) : m_name(name) {}
