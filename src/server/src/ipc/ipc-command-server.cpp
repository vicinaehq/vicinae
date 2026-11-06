#include "ipc/ipc-command-server.hpp"
#include "ipc/browser-peer.hpp"
#include "command/argument.hpp"
#include "command/command-types.hpp"
#include "common/entrypoint.hpp"
#include "common/enumerate.hpp"
#include "generated/ipc-server.hpp"
#include "ipc/ipc-command-handler.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/browser-extension-service.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-service.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include "ui/quick/attachment-model.hpp"
#include "ui/views/dmenu-view-host.hpp"
#include "utils.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <QFutureWatcher>
#include <QPromise>
#include <QTimer>
#include <QtConcurrentRun>
#include <qfuture.h>
#include <qlogging.h>
#include <ranges>
#include <sstream>
#include <string_view>
#include <utility>
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "navigation-controller.hpp"
#include "generated/version.h"

// IpcService

IpcService::IpcService(ipc_gen::RpcTransport &transport, ApplicationContext &ctx)
    : ipc_gen::AbstractIpc(transport), m_ctx(ctx) {}

namespace {

using AttachmentResults = std::vector<AttachmentModel::Result>;

std::expected<AI::ModelRef, std::string> resolveAskModel(AI::Service &ai, const config::Manager &config,
                                                         const std::optional<std::string> &requested) {
  if (requested) {
    auto ref = AI::ModelRef::fromString(*requested);
    if (!ref)
      return std::unexpected(
          std::format("{} is not a valid model id, expected <provider>:<model>", *requested));
    if (!ai.isAvailable(*ref, AI::Completion)) {
      return std::unexpected(std::format(
          "Model {} is not available, run `vicinae ai models` to list the usable ones", *requested));
    }
    return *ref;
  }

  auto ref = AI::defaultModel(config);
  if (!ref || !ai.isAvailable(*ref, AI::Completion)) {
    return std::unexpected("No default AI model is selected, pick one in the AI settings or pass --model");
  }
  return *ref;
}

std::expected<AI::ChatCompletionPayload, std::string> buildAskPayload(const ipc_gen::AiAskRequest &req,
                                                                      AttachmentResults attachments) {
  AI::ChatCompletionPayload payload{.thinking = AI::ThinkingMode::Low, .oneShot = true};
  if (req.system) payload.messages.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::System, *req.system));

  AI::ChatMessage message{.role = AI::ChatRole::User};
  message.parts.reserve(2 + attachments.size() * 2);
  if (req.context) message.parts.emplace_back(AI::TextPart{*req.context});
  message.parts.emplace_back(AI::TextPart{req.prompt});

  for (auto &&[idx, result] : attachments | vicinae::enumerate) {
    if (!result)
      return std::unexpected(std::format("{}: {}", req.attachments[idx], result.error().toStdString()));
    auto &attachment = *result;
    if (auto *image = std::get_if<FileAttachment::Image>(&attachment.contents)) {
      message.parts.emplace_back(AI::TextPart{std::format("Image: {}", attachment.name)});
      message.parts.emplace_back(AI::ImagePart{std::move(image->mimeType), std::move(image->base64)});
    } else {
      message.parts.emplace_back(AI::TextPart{std::format("Attached text file: {}\n\n{}", attachment.name,
                                                          std::get<std::string>(attachment.contents))});
    }
  }

  payload.messages.emplace_back(std::move(message));
  return payload;
}

bool hasImagePart(const AI::ChatCompletionPayload &payload) {
  return std::ranges::any_of(payload.messages, [](const AI::ChatMessage &message) {
    return std::ranges::any_of(
        message.parts, [](const AI::ChatPart &part) { return std::holds_alternative<AI::ImagePart>(part); });
  });
}

} // namespace

ipc_gen::Result<ipc_gen::AiListModelsResponse>::Future IpcService::aiListModels() {
  auto models = m_ctx.services->ai()->listModels(AI::Completion) |
                std::views::transform([](const AI::ProviderModel &model) {
                  return ipc_gen::AiModel{.id = model.ref.toString(),
                                          .name = model.name,
                                          .capabilities = AI::stringifyCapabilities(model.caps)};
                }) |
                std::ranges::to<std::vector>();

  return ipc_gen::Result<ipc_gen::AiListModelsResponse>::ok({.models = std::move(models)});
}

ipc_gen::Result<ipc_gen::AiAskResponse>::Future IpcService::aiAsk(ipc_gen::AiAskRequest req) {
  using Response = ipc_gen::Result<ipc_gen::AiAskResponse>;
  auto &ai = *m_ctx.services->ai();

  auto model = resolveAskModel(ai, *m_ctx.services->config(), req.model);
  if (!model) return Response::fail(model.error());

  QPromise<Response::Type> promise;
  auto future = promise.future();

  // Files are read off the main thread, the completion starts once they are all in
  auto *watcher = new QFutureWatcher<AttachmentResults>(this);
  watcher->setFuture(QtConcurrent::run([paths = req.attachments] {
    AttachmentResults results;
    results.reserve(paths.size());
    for (const auto &path : paths) {
      results.emplace_back(AttachmentModel::readFile(QString::fromStdString(path)));
    }
    return results;
  }));

  QObject::connect(
      watcher, &QFutureWatcher<AttachmentResults>::finished, this,
      [this, watcher, req = std::move(req), model = std::move(*model),
       promise = std::move(promise)]() mutable {
        watcher->deleteLater();
        const auto finish = [&promise](Response::Type result) {
          promise.addResult(std::move(result));
          promise.finish();
        };

        auto payload = buildAskPayload(req, watcher->result());
        if (!payload) return finish(std::unexpected(payload.error()));

        auto &ai = *m_ctx.services->ai();
        if (hasImagePart(*payload) && !ai.isAvailable(model, AI::Vision)) {
          return finish(std::unexpected(std::format("Model {} does not support images", model.toString())));
        }

        auto completion = ai.createChatCompletion(model, *payload);
        if (!completion) {
          return finish(std::unexpected(std::format("Model {} is not available", model.toString())));
        }

        const auto id = std::format("completion-{}", m_completionIdx++);
        QObject::connect(completion.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
                         [this, id](const std::string &data) { emitaiData(id, data, false); });
        QObject::connect(completion.get(), &AI::AbstractChatCompletionStream::finished, this, [this, id]() {
          emitaiData(id, "", true);
          releaseCompletion(id);
        });
        QObject::connect(completion.get(), &AI::AbstractChatCompletionStream::errorOccurred, this,
                         [this, id](const std::string &error) {
                           emitaiFailed(id, error);
                           releaseCompletion(id);
                         });

        m_completions.insert({id, completion});

        // The id must reach the client before anything start() may emit synchronously
        finish(ipc_gen::AiAskResponse{.completionId = id, .model = model.toString()});

        if (!completion->start()) {
          emitaiFailed(id, std::format("Could not start a completion with {}", model.toString()));
          releaseCompletion(id);
        }
      });

  return future;
}

// Runs from the stream's own signals, which can't destroy it before they return
void IpcService::releaseCompletion(const std::string &id) {
  const auto it = m_completions.find(id);
  if (it == m_completions.end()) return;
  QTimer::singleShot(0, this, [stream = std::move(it->second)] {});
  m_completions.erase(it);
}

namespace {

struct FileCategoryDefinition {
  std::string_view name;
  vicinae::FileCategory category;
};

constexpr FileCategoryDefinition FILE_CATEGORIES[] = {
    {.name = "other", .category = vicinae::FileCategory::Other},
    {.name = "directory", .category = vicinae::FileCategory::Directory},
    {.name = "image", .category = vicinae::FileCategory::Image},
    {.name = "video", .category = vicinae::FileCategory::Video},
    {.name = "audio", .category = vicinae::FileCategory::Audio},
    {.name = "document", .category = vicinae::FileCategory::Document},
    {.name = "archive", .category = vicinae::FileCategory::Archive},
    {.name = "application", .category = vicinae::FileCategory::Application},
};

std::optional<vicinae::FileCategory> fileCategoryFromString(std::string_view category) {
  for (const auto &definition : FILE_CATEGORIES) {
    if (definition.name == category) return definition.category;
  }
  return std::nullopt;
}

std::string_view fileCategoryToString(vicinae::FileCategory category) {
  for (const auto &definition : FILE_CATEGORIES) {
    if (definition.category == category) return definition.name;
  }
  return "other";
}

std::expected<ArgumentValues, std::string> buildLaunchArguments(const EntrypointId &id,
                                                                const ArgumentList &manifestArgs,
                                                                const std::vector<std::string> &values) {
  const auto fail = [&](auto &&message) {
    std::ostringstream oss;

    oss << message << "\n" << "Usage: vicinae cmd launch " << std::string{id};

    for (const auto &[idx, arg] : manifestArgs | vicinae::enumerate) {
      auto openChar = arg.required ? '<' : '[';
      auto closeChar = arg.required ? '>' : ']';
      oss << " " << openChar << arg.name.toStdString() << closeChar;
    }

    return std::unexpected(oss.str());
  };

  if (values.size() > manifestArgs.size()) {
    return fail(
        std::format("Too many arguments: expected at most {}, got {}", manifestArgs.size(), values.size()));
  }

  ArgumentValues arguments;
  arguments.reserve(values.size());

  for (const auto [idx, value] : values | vicinae::enumerate) {
    const auto &arg = manifestArgs.at(idx);
    const QString qvalue = QString::fromStdString(value);

    if (arg.type == CommandArgument::Dropdown && arg.data) {
      auto matchesValue = [&](const CommandArgument::DropdownData &option) { return option.value == qvalue; };
      if (!std::ranges::any_of(*arg.data, matchesValue)) {
        return fail(std::format("Invalid value '{}' for argument '{}'", value, arg.name.toStdString()));
      }
    }

    arguments.emplace_back(arg.name, qvalue);
  }

  for (size_t idx = values.size(); idx != manifestArgs.size(); ++idx) {
    const auto &arg = manifestArgs.at(idx);
    if (arg.required) { return fail(std::format("Missing required argument '{}'", arg.name.toStdString())); }
  }

  return arguments;
}

LaunchContext makeLaunchContext(const ipc_gen::LaunchCommandRequest &req) { return LaunchContext{}; }

} // namespace

ipc_gen::Result<ipc_gen::PingResponse>::Future IpcService::ping() {
  return ipc_gen::Result<ipc_gen::PingResponse>::ok(
      {.version = VICINAE_GIT_TAG, .pid = static_cast<int>(QCoreApplication::applicationPid())});
}

ipc_gen::Result<ipc_gen::DeeplinkResponse>::Future IpcService::deeplink(ipc_gen::DeeplinkRequest req) {
  IpcCommandHandler handler(m_ctx);
  QUrl const url(req.url.c_str());

  if (!url.isValid()) return ipc_gen::Result<ipc_gen::DeeplinkResponse>::fail("Not a valid URL");

  const auto res = handler.handleUrl(url);
  if (!res) return ipc_gen::Result<ipc_gen::DeeplinkResponse>::fail(res.error());

  return ipc_gen::Result<ipc_gen::DeeplinkResponse>::ok({});
}

ipc_gen::Result<ipc_gen::DescribeResponse>::Future IpcService::describe() {
  return ipc_gen::Result<ipc_gen::DescribeResponse>::ok(
      {.open = m_ctx.navigation->isWindowOpened(),
       .entrypoint = m_ctx.navigation->activeCommand()->uniqueId()});
}

ipc_gen::Result<std::vector<ipc_gen::FileResult>>::Future IpcService::fsQuery(std::string q,
                                                                              ipc_gen::FsQueryParams params) {
  auto files = m_ctx.services->fileService();
  IndexerQueryParams queryParams{.limit = params.limit};

  if (params.category) {
    auto category = fileCategoryFromString(*params.category);
    if (!category) {
      return ipc_gen::Result<std::vector<ipc_gen::FileResult>>::fail("Unknown file category: " +
                                                                     *params.category);
    }
    queryParams.category = *category;
  }

  return files->queryAsync(q, queryParams).then([](const std::vector<IndexerFileResult> &results) {
    auto fileResults =
        results | std::views::transform([](const IndexerFileResult &result) {
          return ipc_gen::FileResult{.path = result.path.string(),
                                     .score = static_cast<std::uint32_t>(result.rank),
                                     .category = std::string{fileCategoryToString(result.category)},
                                     .mimeType = result.mimeType};
        }) |
        std::ranges::to<std::vector>();

    return std::expected<std::vector<ipc_gen::FileResult>, std::string>{std::move(fileResults)};
  });
}

ipc_gen::Result<ipc_gen::LaunchAppResponse>::Future IpcService::launchApp(ipc_gen::LaunchAppRequest req) {
  auto appDb = m_ctx.services->appDb();
  auto wm = m_ctx.services->windowManager();
  auto app = appDb->findById(req.appId.c_str());

  if (!app) return ipc_gen::Result<ipc_gen::LaunchAppResponse>::fail("No app with id");

  if (!req.newInstance) {
    if (auto wins = wm->findAppWindows(*app); !wins.empty()) {
      auto &win = wins.front();
      wm->provider()->focusWindowSync(*win);
      return ipc_gen::Result<ipc_gen::LaunchAppResponse>::ok(
          {.focusedWindowTitle = win->title().toStdString()});
    }
  }

  std::vector<QString> const args = Utils::toQStringVec(req.args);

  if (!appDb->launch(*app, args))
    return ipc_gen::Result<ipc_gen::LaunchAppResponse>::fail(
        std::format("Failed to launch app with id {}", req.appId));

  return ipc_gen::Result<ipc_gen::LaunchAppResponse>::ok({});
}

ipc_gen::Result<ipc_gen::ListCommandsResponse>::Future IpcService::listCommands() {
  auto root = m_ctx.services->rootItemManager();
  auto commands =
      root->allItems() | std::views::transform([](auto &&item) {
        return ipc_gen::CommandInfo{.id = item.item->uniqueId(), .name = item.item->title().toStdString()};
      }) |
      std::ranges::to<std::vector>();

  std::ranges::sort(commands, [](const auto &a, const auto &b) { return a.id < b.id; });

  return ipc_gen::Result<ipc_gen::ListCommandsResponse>::ok({.commands = std::move(commands)});
}

ipc_gen::Result<ipc_gen::LaunchCommandResponse>::Future
IpcService::launchCommand(ipc_gen::LaunchCommandRequest req) {
  auto id = EntrypointId::fromSerialized(req.entrypoint);

  if (!id.isValid()) {
    return ipc_gen::Result<ipc_gen::LaunchCommandResponse>::fail(
        std::format("Ill-formed command entrypoint: {}", req.entrypoint));
  }

  auto root = m_ctx.services->rootItemManager();
  auto *item = root->findItemById(id);

  if (!item) {
    return ipc_gen::Result<ipc_gen::LaunchCommandResponse>::fail(
        std::format("Unknown command entrypoint: {}", req.entrypoint));
  }

  auto arguments = buildLaunchArguments(id, item->arguments(), req.args);

  if (!arguments) return ipc_gen::Result<ipc_gen::LaunchCommandResponse>::fail(arguments.error());

  LaunchProps props{
      .arguments = std::move(arguments).value(),
      .launchContext = makeLaunchContext(req),
      .fallbackText = req.query.transform(QString::fromStdString),
      .cwd = req.cwd.transform(QString::fromStdString),
      .source = LaunchSource::External,
  };

  if (!m_ctx.navigation->activateEntrypoint(id, {.props = props, .toggleIfAlreadyActive = false})) {
    return ipc_gen::Result<ipc_gen::LaunchCommandResponse>::fail("Failed to launch command");
  }

  return ipc_gen::Result<ipc_gen::LaunchCommandResponse>::ok({});
}

ipc_gen::Result<ipc_gen::DMenuResponse>::Future IpcService::dmenu(ipc_gen::DMenuRequest request) {
  static constexpr const int DMENU_SMALL_WIDTH_THRESHOLD = 500;
  auto &nav = m_ctx.navigation;
  auto &cfg = m_ctx.services->config()->value();

  QPromise<ipc_gen::Result<ipc_gen::DMenuResponse>::Type> promise;
  auto future = promise.future();

  if (request.width.has_value() && request.width.value() < DMENU_SMALL_WIDTH_THRESHOLD) {
    qInfo() << "dmenu: disabling quicklook and footer because width is too low";
    request.noFooter = true;
    request.noQuickLook = true;
  }

  // Convert to the DMenu-specific request type used by the view host
  ipc_gen::DMenuRequest viewReq = std::move(request);
  auto view = new DMenuViewHost(viewReq);

  using Watcher = QFutureWatcher<ipc_gen::Result<ipc_gen::DMenuResponse>::Type>;
  auto watcher = new Watcher;
  watcher->setFuture(future);

  QObject::connect(watcher, &Watcher::canceled, [nav = nav.get()]() { nav->closeWindow(); });
  QObject::connect(watcher, &Watcher::finished, [watcher]() { watcher->deleteLater(); });
  QObject::connect(view, &DMenuViewHost::selected,
                   [promise = std::move(promise)](const QString &text) mutable {
                     promise.addResult(ipc_gen::DMenuResponse{.output = text.toStdString()});
                     promise.finish();
                   });

  nav->popToRoot({.clearSearch = false});
  nav->pushView(view);
  nav->setInstantDismiss(true);

  if (request.width || request.height) {
    int const w = request.width.value_or(cfg.launcherWindow.size.width);
    int const h = request.height.value_or(cfg.launcherWindow.size.height);
    nav->requestWindowSize(QSize(w, h));
  }

  nav->showWindow();

  return future;
}

ipc_gen::Result<void>::Future IpcService::browserInit(ipc_gen::BrowserInitRequest req) {
  std::shared_ptr<AbstractBrowserPeer> peer;

  if (req.protocolVersion.value_or(1) >= BROWSER_PROTOCOL_VERSION) {
    peer = std::make_shared<IpcBrowserPeer>(*this);
  } else {
    qWarning() << "Browser extension in" << req.name.c_str() << "is too old, it needs to be updated";
  }

  m_ctx.services->browserExtension()->registerBrowser({.id = req.id,
                                                       .name = req.name,
                                                       .engine = req.engine,
                                                       .version = req.version,
                                                       .permissions = req.permissions,
                                                       .optionalPermissions = req.optionalPermissions,
                                                       .peer = std::move(peer)});
  m_browser = std::move(req);

  return ipc_gen::Result<void>::ok();
}

ipc_gen::Result<void>::Future IpcService::browserTabsChanged(std::vector<ipc_gen::BrowserTabInfo> tabs) {
  if (!m_browser) return ipc_gen::Result<void>::fail("Only browser extensions can do this");

  m_ctx.services->browserExtension()->setTabs(m_browser->id, tabs);

  return ipc_gen::Result<void>::ok();
}

ipc_gen::Result<void>::Future IpcService::browserPermissionsChanged(std::vector<std::string> permissions) {
  if (!m_browser) return ipc_gen::Result<void>::fail("Only browser extensions can do this");

  m_ctx.services->browserExtension()->setPermissions(m_browser->id, std::move(permissions));

  return ipc_gen::Result<void>::ok();
}

// IpcConnection

void IpcConnection::SocketTransport::send(std::string_view data) {
  const uint32_t size = data.size();
  socket->write(reinterpret_cast<const char *>(&size), sizeof(size));
  socket->write(data.data(), data.size());
}

IpcConnection::IpcConnection(QLocalSocket *socket, ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_socket(socket), m_transport(socket), m_rpc(m_transport),
      m_service(new IpcService(m_rpc, ctx)), m_server(m_rpc, m_service) {
  m_socket->setParent(this);
  connect(m_socket, &QLocalSocket::readyRead, this, &IpcConnection::handleRead);
}

void IpcConnection::handleRead() {
  while (m_socket->bytesAvailable() > 0) {
    m_frame.append(m_socket->readAll());

    while (std::cmp_greater_equal(m_frame.size(), sizeof(uint32_t))) {
      uint32_t const length = *reinterpret_cast<uint32_t *>(m_frame.data());
      bool const isComplete = m_frame.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      const auto packet = QByteArrayView(m_frame).sliced(sizeof(uint32_t), length);

      m_server.route({packet.data(), static_cast<size_t>(packet.size())});

      m_frame = m_frame.sliced(sizeof(uint32_t) + length);
    }
  }
}

// IpcCommandServer

IpcCommandServer::IpcCommandServer(ApplicationContext *ctx, QObject *parent) : QObject(parent), m_ctx(*ctx) {}

void IpcCommandServer::handleDisconnection(IpcConnection *connection) {
  // drop the browser first, so that nothing calls into the connection while it is torn down
  if (const auto &browser = connection->service()->browser()) {
    m_ctx.services->browserExtension()->unregisterBrowser(browser->id);
  }

  connection->deleteLater();
}

void IpcCommandServer::handleConnection() {
  QLocalSocket *socket = m_server.nextPendingConnection();

  qDebug() << "ipc command server received new connection";

  auto *connection = new IpcConnection(socket, m_ctx, this);
  connect(socket, &QLocalSocket::disconnected, this, [this, connection] { handleDisconnection(connection); });
}

bool IpcCommandServer::start(const std::string &name) {
#ifndef _WIN32
  // Named pipes have no filesystem entry; only a POSIX socket file can go stale and block listen().
  if (std::filesystem::exists(name)) { std::filesystem::remove(name); }
#endif

  if (!m_server.listen(QString::fromStdString(name))) {
    qDebug() << "CommandServer failed to listen" << m_server.errorString();
    return false;
  }

  connect(&m_server, &QLocalServer::newConnection, this, &IpcCommandServer::handleConnection);

  qDebug() << "Server started, listening on:" << name.c_str();

  return true;
}
