#pragma once
#include <span>
#include <QString>
#include "services/ai/text-rewrite.hpp"
#include "services/dictation/dictation-styles.hpp"
#include "services/dictation/dictation-vocabulary.hpp"

namespace Dictation {

/**
 * The request that rewrites one transcript with a style.
 */
TextRewrite::Request rewriteRequest(const DictationStyles::Style &style,
                                    std::span<const DictationVocabulary::Entry> vocabulary,
                                    const QString &transcript);

} // namespace Dictation
