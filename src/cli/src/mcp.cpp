#include "mcp.hpp"
#include <array>
#include <cstring>
#include <format>
#include "common/common.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <unistd.h>
#endif

namespace cli {

static constexpr const char *HELPER_PROGRAM = "vicinae-mcp";

#ifdef _WIN32

std::expected<int, std::string> runMcpServer() {
  const auto path = vicinae::findHelperProgram(HELPER_PROGRAM);
  if (!path) return std::unexpected(std::format("Could not find the {} helper", HELPER_PROGRAM));

  HANDLE job = CreateJobObjectW(nullptr, nullptr);
  if (!job) return std::unexpected(std::format("Failed to create a job object: error {}", GetLastError()));

  JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
  limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits));

  STARTUPINFOW startup{.cb = sizeof(STARTUPINFOW),
                       .dwFlags = STARTF_USESTDHANDLES,
                       .hStdInput = GetStdHandle(STD_INPUT_HANDLE),
                       .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
                       .hStdError = GetStdHandle(STD_ERROR_HANDLE)};
  PROCESS_INFORMATION process{};
  std::wstring commandLine = std::format(L"\"{}\"", path->wstring());

  if (!CreateProcessW(path->c_str(), commandLine.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr,
                      nullptr, &startup, &process)) {
    const auto error = GetLastError();
    CloseHandle(job);
    return std::unexpected(std::format("Failed to start {}: error {}", HELPER_PROGRAM, error));
  }

  AssignProcessToJobObject(job, process.hProcess);
  ResumeThread(process.hThread);
  CloseHandle(process.hThread);

  WaitForSingleObject(process.hProcess, INFINITE);
  DWORD code = 1;
  GetExitCodeProcess(process.hProcess, &code);
  CloseHandle(process.hProcess);
  CloseHandle(job);
  return static_cast<int>(code);
}

#else

std::expected<int, std::string> runMcpServer() {
  const auto path = vicinae::findHelperProgram(HELPER_PROGRAM);
  if (!path) return std::unexpected(std::format("Could not find the {} helper", HELPER_PROGRAM));

  const std::string name = path->filename().string();
  const auto argv = std::to_array<char *>({const_cast<char *>(name.c_str()), nullptr});
  execv(path->c_str(), argv.data());
  return std::unexpected(std::format("Failed to start {}: {}", HELPER_PROGRAM, std::strerror(errno)));
}

#endif

} // namespace cli
