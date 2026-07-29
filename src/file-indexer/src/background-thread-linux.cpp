#include "file-indexer/background-thread.hpp"
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace {
constexpr int IOPRIO_WHO_PROCESS = 1;
constexpr int IOPRIO_CLASS_IDLE = 3;
constexpr int IOPRIO_CLASS_SHIFT = 13;
} // namespace

namespace file_indexer {

void setBackgroundThreadPriority() {
  auto const tid = static_cast<id_t>(syscall(SYS_gettid));

  // on Linux setpriority() with a tid is thread-granular
  setpriority(PRIO_PROCESS, tid, 19);
  // who=0 targets the calling thread. nvme use the 'none' scheduler so this does nothing for them
  syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0, IOPRIO_CLASS_IDLE << IOPRIO_CLASS_SHIFT);
}

} // namespace file_indexer
