#pragma once
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <QObject>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include "utils/json-file.hpp"

/**
 * User-defined AI commands, kept in one JSON file.
 */
class AiCommandStore : public QObject {
  Q_OBJECT

signals:
  void changed();

public:
  enum class Output { Paste, Copy };

  struct Command {
    std::string id;
    std::string name;
    std::string icon;
    /** Sent to the model, with placeholders such as {selection} expanded first. */
    std::string prompt;
    std::string model;
    Output output = Output::Paste;
  };

  explicit AiCommandStore(std::filesystem::path path, QObject *parent = nullptr);

  std::span<const Command> entries() const { return m_file.data().commands; }
  const Command *find(std::string_view id) const;
  const Command *findByName(std::string_view name) const;

  /**
   * Returns the id of the new command.
   */
  std::string add(Command command);
  bool update(Command command);
  bool remove(std::string_view id);

private:
  struct Data {
    std::vector<Command> commands;
  };

  bool commit();

  JsonFile<Data> m_file;
};

template <> struct glz::meta<AiCommandStore::Output> {
  using enum AiCommandStore::Output;
  static constexpr auto value = glz::enumerate("paste", Paste, "copy", Copy);
};
