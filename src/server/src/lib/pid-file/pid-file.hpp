#pragma once
#include <filesystem>
#include <string>

class PidFile {
public:
  bool kill();
  void write(int pid);
  std::filesystem::path path() const;
  bool exists();

  /**
   * Return currently stored pid if any.
   */
  std::optional<int> pid();

  PidFile(const std::string &name);

private:
  std::string m_name;
};
