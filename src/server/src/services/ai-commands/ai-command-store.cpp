#include "ai-command-store.hpp"
#include <algorithm>
#include <utility>
#include <QUuid>

AiCommandStore::AiCommandStore(std::filesystem::path path, QObject *parent)
    : QObject(parent), m_file(std::move(path)) {}

const AiCommandStore::Command *AiCommandStore::find(std::string_view id) const {
  const auto commands = entries();
  const auto it = std::ranges::find(commands, id, &Command::id);
  return it == commands.end() ? nullptr : &*it;
}

const AiCommandStore::Command *AiCommandStore::findByName(std::string_view name) const {
  const auto commands = entries();
  const auto it = std::ranges::find(commands, name, &Command::name);
  return it == commands.end() ? nullptr : &*it;
}

std::string AiCommandStore::add(Command command) {
  command.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
  auto id = command.id;
  m_file.data().commands.emplace_back(std::move(command));
  commit();
  return id;
}

bool AiCommandStore::update(Command command) {
  auto &commands = m_file.data().commands;
  const auto it = std::ranges::find(commands, command.id, &Command::id);
  if (it == commands.end()) return false;
  *it = std::move(command);
  return commit();
}

bool AiCommandStore::remove(std::string_view id) {
  if (std::erase_if(m_file.data().commands, [&](const Command &command) { return command.id == id; }) == 0) {
    return false;
  }
  return commit();
}

bool AiCommandStore::commit() {
  const bool saved = m_file.save();
  emit changed();
  return saved;
}
