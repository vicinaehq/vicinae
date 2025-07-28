#pragma once
#include <memory>
#include <variant>
#include "ui/action-pannel/action-label.hpp"

class AbstractAction;
class AbstractActionSection;

using ActionItem = std::variant<ActionLabel, std::shared_ptr<AbstractAction>>;
