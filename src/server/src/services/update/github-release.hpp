#pragma once
#include <string>
#include <vector>

namespace github {

struct ReleaseAsset {
  std::string name;
  std::string browser_download_url;
};

struct Release {
  std::string tag_name;
  std::string html_url;
  bool draft = false;
  bool prerelease = false;
  std::vector<ReleaseAsset> assets;
};

} // namespace github
