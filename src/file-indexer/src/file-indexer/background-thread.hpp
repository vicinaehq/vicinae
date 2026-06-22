#pragma once

namespace file_indexer {

// Lowest CPU and IO scheduling priority for the calling thread. Best effort.
void setBackgroundThreadPriority();

} // namespace file_indexer
