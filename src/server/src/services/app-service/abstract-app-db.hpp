#pragma once
#include "ui/image/url.hpp"
#include <QString>
#include <optional>
#include <qmimetype.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <quuid.h>
#include "proto/application.pb.h"

class AbstractApplication {
public:
  /**
   * Unique identifier for this app.
   */
  virtual QString id() const = 0;

  virtual QString displayName() const = 0;

  virtual bool displayable() const = 0;

  /**
   * Whether the application should run in a terminal emulator.
   */
  virtual bool isTerminalApp() const = 0;

  /**
   * Whether the application IS a terminal emulator.
   */
  virtual bool isTerminalEmulator() const { return false; }

  /**
   * A longer name than the default one, conveying additional details (if applicable).
   * This would typically make sense for an action that could embed the original application
   * name before the action name, such as: 'firefox: open in private window' instead of simply
   * `open in private window`.
   */
  virtual QString fullyQualifiedName() const { return displayName(); }

  virtual ImageURL iconUrl() const = 0;

  /**
   * Whether this application is an alternative action for an existing
   * application.
   */
  virtual bool isAction() const { return false; }

  /**
   * List of additional actions or commands the app can execute.
   * Note that every action is considered as its own app, although they are not displayed in the root
   * search directly.
   * Note that every action should be searchable by its full id using the app service's `findById` method.
   * It is expected that an action can live independently from its owning application.
   * Currently, on the XdgApplicationDatabase supports this.
   */
  virtual std::vector<std::shared_ptr<AbstractApplication>> actions() const { return {}; }

  /**
   * Additional keywords that can be used to index the application.
   */
  virtual std::vector<QString> keywords() const { return {}; }

  /**
   * Path to the application bundle or manifest.
   * What this actually points to is very implementation dependant: on Linux it points
   * to the .desktop entry, on Mac to the application bundle, etc...
   */
  virtual std::filesystem::path path() const = 0;

  /**
   * The name or path of the program that this application runs.
   */
  virtual QString program() const = 0;

  virtual std::optional<QString> windowClass() const { return std::nullopt; }

  virtual bool matchesWindowClass(const QString &wmClass) const { return false; }

  /**
   * A short multiline description that explains what the app does.
   */
  virtual QString description() const = 0;

  // whether the executable can open url(s) or file(s)
  virtual bool isOpener() { return true; }

  proto::ext::application::Application toProto() {
    proto::ext::application::Application app;

    app.set_name(displayName().toStdString());
    app.set_icon(iconUrl().name().toStdString());
    app.set_id(id().toStdString());
    app.set_path(path());

    return app;
  }
};

struct LaunchTerminalCommandOptions {
  bool hold = false;
  std::optional<QString> appId;
  std::optional<QString> workingDirectory;
  std::optional<QString> title;
  // default will be chosen if not specified
  AbstractApplication *emulator = nullptr;
};

class AbstractAppDatabase : public QObject {
public:
  using AppPtr = std::shared_ptr<AbstractApplication>;

  /**
   * A target is a string that can be an URI, a path to a file or something else entirely.
   * Wherever a target is passed, the method should do its best to find openers for it.
   */
  using Target = QString;

  /**
   * The default system search paths to use.
   * Unlike paths that are manually added from the preferences menu, these cannot be edited.
   */
  virtual std::vector<std::filesystem::path> defaultSearchPaths() const = 0;

  /**
   * This method is a request for the service to explicitly update the list of apps installed on the system.
   * What it does is left to the implementer but it is expected that after calling this, we can query the
   * service for the most up-to-date information.
   * Usually, implementers install their own watching logic to detect changes and rebuild their app database
   * internally, but this is called when a higher level operation that could impact apps is performed (such as
   * changing app-related preferences).
   */
  virtual bool scan(const std::vector<std::filesystem::path> &paths) = 0;

  /**
   * Launch an instance of the application with the provided set of arguments.
   * How this is done is very implementation dependent but a few things are to be kept in mind:
   * - Make sure the process is running detached, as we don't want the launched processed to be killed if
   * vicinae is.
   * - You should trash stdout and stderr as vicinae should not print application logs.
   */
  virtual bool launch(const AbstractApplication &exec, const std::vector<QString> &args = {},
                      const std::optional<QString> &launchPrefix = {}) const = 0;

  /**
   * Launch the specified command in the default terminal.
   */
  virtual bool launchTerminalCommand(const std::vector<QString> &cmdline,
                                     const LaunchTerminalCommandOptions &opts = {},
                                     const std::optional<QString> &prefix = {}) const = 0;

  /**
   * Find all the possible openers for the given target, from most to least preferred.
   * It is expected that the most preferred opener (the first one in the list) is also the
   * default opener for the target.
   */
  virtual std::vector<AppPtr> findOpeners(const Target &target) const = 0;

  /**
   * Find the default opener for the given target, or a null pointer if none could be found.
   */
  virtual AppPtr findDefaultOpener(const Target &target) const = 0;

  virtual AppPtr findById(const QString &id) const = 0;

  /**
   * The list of available applications, including applications that have their `displayable` method return
   * false. It's the responsability of the consumer to ignore the non displayable apps where needed.
   */
  virtual std::vector<AppPtr> list() const = 0;

  virtual AppPtr findByClass(const QString &name) const = 0;

  /**
   * The preferred file browser to use to open directories.
   */
  virtual AppPtr fileBrowser() const = 0;

  /**
   * The preferred text editor to use to open generic plain text files.
   */
  virtual AppPtr genericTextEditor() const = 0;

  /**
   * The preferred web browser to use on this system.
   */
  virtual AppPtr webBrowser() const = 0;

  /**
   * The preferred terminal emulator to use on this system.
   */
  virtual AppPtr terminalEmulator() const = 0;
};
