#pragma once
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <QObject>

namespace AI {

class AbstractTool;
class ToolRegistry;

class McpServer : public QObject {
  Q_OBJECT

signals:
  void messageReady(const std::string &message);

public:
  explicit McpServer(ToolRegistry &registry, QObject *parent = nullptr);
  ~McpServer() override;

  void receive(std::string_view message);

private:
  struct Call {
    std::string requestId;
    std::unique_ptr<AbstractTool> tool;
    std::function<void()> cancel;
  };

  void handleRequest(const std::string &id, std::string_view method, std::string_view params);
  void initialize(const std::string &id, std::string_view params);
  void listTools(const std::string &id);
  void callTool(const std::string &id, std::string_view params);
  void cancel(std::string_view params);
  void finishCall(const std::string &id, std::string content, bool failed);
  void respondCall(const std::string &id, std::string content, bool failed);
  void respond(const std::string &id, std::string_view result);
  void fail(const std::string &id, int code, std::string_view message);
  void notifyListChanged();
  std::vector<std::string> exportedTools() const;

  ToolRegistry &m_registry;
  std::vector<std::string> m_listed;
  std::vector<Call> m_calls;
  bool m_initialized = false;
};

} // namespace AI
