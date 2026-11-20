#include "mcp-server.hpp"
#include <algorithm>
#include <array>
#include <format>
#include <optional>
#include <ranges>
#include <glaze/json.hpp>
#include <QCoreApplication>
#include <QFutureWatcher>
#include "generated/version.h"
#include "services/ai/ai-tool.hpp"
#include "services/ai/tool-registry.hpp"

namespace AI {

// not anonymous: glaze can't reflect types with internal linkage
namespace mcp {

struct Incoming {
  std::optional<glz::raw_json> id;
  std::optional<std::string> method;
  std::optional<glz::raw_json> params;
};

struct InitializeParams {
  std::optional<std::string> protocolVersion;
};

struct ToolsCapability {
  bool listChanged = true;
};

struct ServerCapabilities {
  ToolsCapability tools;
};

struct ServerInfo {
  std::string name;
  std::string title;
  std::string version;
};

struct InitializeResult {
  std::string protocolVersion;
  ServerCapabilities capabilities;
  ServerInfo serverInfo;
};

struct ToolAnnotations {
  std::optional<bool> readOnlyHint;
  std::optional<bool> destructiveHint;
  std::optional<bool> idempotentHint;
  std::optional<bool> openWorldHint;
};

struct Tool {
  std::string name;
  std::string title;
  std::string description;
  glz::raw_json inputSchema;
  ToolAnnotations annotations;
};

struct ListToolsResult {
  std::vector<Tool> tools;
};

struct CallToolParams {
  std::string name;
  std::optional<glz::raw_json> arguments;
};

struct TextContent {
  std::string type = "text";
  std::string text;
};

struct CallToolResult {
  std::vector<TextContent> content;
  bool isError = false;
};

struct CancelledParams {
  glz::raw_json requestId;
};

struct Error {
  int code;
  std::string message;
};

struct ResultMessage {
  std::string jsonrpc = "2.0";
  glz::raw_json id;
  glz::raw_json result;
};

struct ErrorMessage {
  std::string jsonrpc = "2.0";
  glz::raw_json id;
  Error error;
};

} // namespace mcp

namespace {

constexpr glz::opts LENIENT{.error_on_unknown_keys = false};

constexpr auto PROTOCOL_VERSIONS =
    std::to_array<std::string_view>({"2025-11-25", "2025-06-18", "2025-03-26"});

constexpr int METHOD_NOT_FOUND = -32601;
constexpr int INVALID_PARAMS = -32602;

template <typename T> std::string toJson(const T &value) {
  std::string out;
  if (glz::write_json(value, out)) return "{}";
  return out;
}

template <typename T> std::optional<T> fromJson(std::string_view json) {
  T value;
  if (glz::read<LENIENT>(value, json)) return std::nullopt;
  return value;
}

} // namespace

McpServer::McpServer(ToolRegistry &registry, QObject *parent) : QObject(parent), m_registry(registry) {
  connect(&registry, &ToolRegistry::changed, this, &McpServer::notifyListChanged);
}

McpServer::~McpServer() {
  for (auto &call : m_calls) {
    if (call.cancel) call.cancel();
  }
}

void McpServer::receive(std::string_view message) {
  const auto incoming = fromJson<mcp::Incoming>(message);
  if (!incoming || !incoming->method) return;

  const auto params = incoming->params ? std::string_view(incoming->params->str) : std::string_view("{}");
  if (incoming->id) {
    handleRequest(incoming->id->str, *incoming->method, params);
  } else if (*incoming->method == "notifications/initialized") {
    m_initialized = true;
    m_listed = exportedTools();
  } else if (*incoming->method == "notifications/cancelled") {
    cancel(params);
  }
}

void McpServer::handleRequest(const std::string &id, std::string_view method, std::string_view params) {
  if (method == "initialize") return initialize(id, params);
  if (method == "ping") return respond(id, "{}");
  if (method == "tools/list") return listTools(id);
  if (method == "tools/call") return callTool(id, params);
  fail(id, METHOD_NOT_FOUND, std::format("Method not found: {}", method));
}

void McpServer::initialize(const std::string &id, std::string_view params) {
  const auto requested = fromJson<mcp::InitializeParams>(params).and_then(
      [](const mcp::InitializeParams &params) { return params.protocolVersion; });
  const auto version = requested && std::ranges::contains(PROTOCOL_VERSIONS, *requested)
                           ? *requested
                           : std::string(PROTOCOL_VERSIONS.front());

  respond(id, toJson(mcp::InitializeResult{
                  .protocolVersion = version,
                  .serverInfo = {.name = "vicinae", .title = "Vicinae", .version = VICINAE_GIT_TAG},
              }));
}

void McpServer::listTools(const std::string &id) {
  const auto &entries = m_registry.entries();
  mcp::ListToolsResult result;
  result.tools.reserve(entries.size());

  for (const auto &entry : entries) {
    if (!entry.contribution.mcp) continue;
    const auto tool = m_registry.createTool(entry);
    if (!tool || !tool->isEnabled()) continue;
    const auto &annotations = entry.contribution.annotations;
    result.tools.emplace_back(mcp::Tool{.name = entry.name,
                                        .title = entry.contribution.title.toStdString(),
                                        .description = tool->description(),
                                        .inputSchema = tool->generateInputSchema(),
                                        .annotations = {
                                            .readOnlyHint = annotations.readOnly,
                                            .destructiveHint = annotations.destructive,
                                            .idempotentHint = annotations.idempotent,
                                            .openWorldHint = annotations.openWorld,
                                        }});
  }

  m_listed = exportedTools();
  respond(id, toJson(result));
}

void McpServer::callTool(const std::string &id, std::string_view params) {
  const auto call = fromJson<mcp::CallToolParams>(params);
  if (!call) return fail(id, INVALID_PARAMS, "Invalid tool call");

  const auto *entry = m_registry.find(call->name);
  auto tool = entry && entry->contribution.mcp ? m_registry.createTool(*entry) : nullptr;
  if (!tool || !tool->isEnabled())
    return fail(id, INVALID_PARAMS, std::format("Unknown tool: {}", call->name));

  AbstractTool::RawToolTask task;
  try {
    task = tool->runRaw(call->arguments ? call->arguments->str : "{}");
  } catch (const std::exception &error) { return respondCall(id, error.what(), true); }

  m_calls.emplace_back(Call{.requestId = id, .tool = std::move(tool), .cancel = std::move(task.cancel)});

  auto *watcher = new QFutureWatcher<AbstractTool::RawToolResult>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, id] {
    watcher->deleteLater();

    AbstractTool::RawToolResult result = std::unexpected(
        QCoreApplication::translate("McpServer", "The tool did not return a result.").toStdString());
    try {
      if (!watcher->isCanceled() && watcher->future().resultCount() > 0) result = watcher->result();
    } catch (const std::exception &error) { result = std::unexpected(error.what()); }

    if (result) {
      finishCall(id, std::move(result->content), result->failed);
    } else {
      finishCall(id, std::move(result.error()), true);
    }
  });
  watcher->setFuture(task.future);
}

void McpServer::cancel(std::string_view params) {
  const auto cancelled = fromJson<mcp::CancelledParams>(params);
  if (!cancelled) return;

  const auto call = std::ranges::find(m_calls, cancelled->requestId.str, &Call::requestId);
  if (call == m_calls.end()) return;

  auto cancelCall = std::move(call->cancel);
  m_calls.erase(call);
  if (cancelCall) cancelCall();
}

void McpServer::finishCall(const std::string &id, std::string content, bool failed) {
  const auto call = std::ranges::find(m_calls, id, &Call::requestId);
  if (call == m_calls.end()) return;

  m_calls.erase(call);
  respondCall(id, std::move(content), failed);
}

void McpServer::respondCall(const std::string &id, std::string content, bool failed) {
  respond(id, toJson(mcp::CallToolResult{.content = {{.text = std::move(content)}}, .isError = failed}));
}

void McpServer::respond(const std::string &id, std::string_view result) {
  emit messageReady(toJson(mcp::ResultMessage{.id = id, .result = std::string(result)}));
}

void McpServer::fail(const std::string &id, int code, std::string_view message) {
  emit messageReady(
      toJson(mcp::ErrorMessage{.id = id, .error = {.code = code, .message = std::string(message)}}));
}

void McpServer::notifyListChanged() {
  if (!m_initialized) return;

  auto tools = exportedTools();
  if (tools == m_listed) return;

  m_listed = std::move(tools);
  emit messageReady(R"({"jsonrpc":"2.0","method":"notifications/tools/list_changed"})");
}

std::vector<std::string> McpServer::exportedTools() const {
  return m_registry.entries() | std::views::filter([this](const ToolRegistry::Entry &entry) {
           return entry.contribution.mcp && m_registry.isEnabled(entry.name);
         }) |
         std::views::transform(&ToolRegistry::Entry::name) | std::ranges::to<std::vector>();
}

} // namespace AI
