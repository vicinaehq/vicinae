#include <catch2/catch_test_macros.hpp>
#include <ostream>
#include "vicinae-ipc/client.hpp"
#include "vicinae-ipc/ipc.hpp"

TEST_CASE("test") {
  auto client = ipc::Client<ipc::CliSchema>::make().value();

  auto err = client.request<ipc::Deeplink>({.url = "vicinae://toggle"});

  REQUIRE(true);
}
