#pragma once
#include "common/context.hpp"
#include <QObject>
#include <memory>
#include <string>
#include <unordered_map>

class ExtensionCommand;
class ExtensionCommandRuntime;
struct LaunchProps;

/** Runs no-view extension commands without replacing the active navigation frame. */
class ExtensionBackgroundRunner : public QObject {
public:
  explicit ExtensionBackgroundRunner(ApplicationContext &context);
  ~ExtensionBackgroundRunner() override;

  bool launch(const std::shared_ptr<ExtensionCommand> &command, const LaunchProps &props);

private:
  void complete(ExtensionCommandRuntime *runtime);

  ApplicationContext &m_context;
  std::unordered_map<std::string, ExtensionCommandRuntime *> m_runtimes;
};
