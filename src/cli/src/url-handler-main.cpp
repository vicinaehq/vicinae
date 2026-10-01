#include <string>
#include "ipc-client.hpp"
#include "common/common.hpp"

int main(int ac, char **av) {
  vicinae::enableUtf8();

  if (ac != 2) return 1;

  const std::string url = av[1];

  if (!vicinae::isAppDeeplink(url)) return 1;

  return cli::IpcClient::sendDeeplink(url) ? 0 : 1;
}
