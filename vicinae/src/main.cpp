#include "cli/cli.hpp"
#include <QString>

int main(int argc, char **argv) {
  CommandLineInterface::execute(argc, argv);
  return 0;

  /*
  DaemonIpcClient daemonClient;

  if (!daemonClient.connect()) {
    qInfo() << "Vicinae server is not running. Please run 'vicinae server' or "
               "systemctl enable --now --user vicinae.service";
    return 1;
  }

  if (argc == 1) {
    daemonClient.toggle();
    return 0;
  }

  QUrl url(argv[1]);

  if (url.isValid()) {
    daemonClient.passUrl(url);
    return 0;
  }
  */
}
