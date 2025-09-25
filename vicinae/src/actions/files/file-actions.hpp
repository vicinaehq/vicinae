#pragma once
#include "actions/app/app-actions.hpp"
#include "service-registry.hpp"
#include "services/files-service/file-service.hpp"

class OpenFileAction : public OpenAppAction {
public:
  void execute(ApplicationContext *ctx) override {
    auto files = ctx->services->fileService();

    OpenAppAction::execute(ctx);
    files->saveAccess(m_path);
  }

  OpenFileAction(const std::filesystem::path &path, const std::shared_ptr<AbstractApplication> &app)
      : OpenAppAction(app, "Open with " + app->displayName(), {path.c_str()}), m_path(path) {}

private:
  std::filesystem::path m_path;
};
