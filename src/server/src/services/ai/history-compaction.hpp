#pragma once
#include "services/ai/ai-provider.hpp"

namespace AI {

/**
 * What to send a model for this history, which is resent in full at every step of every turn. Old tool
 * output and images are shortened or removed, then the oldest turns dropped if it's still too large. Stored
 * conversations are unaffected.
 */
ChatHistory compactHistory(const ChatHistory &history);

} // namespace AI
