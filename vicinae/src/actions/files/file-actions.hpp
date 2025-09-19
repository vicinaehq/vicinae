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

  OpenFileAction(const std::filesystem::path &path, const std::shared_ptr<Application> &app)
      : OpenAppAction(app, "Open", {path.c_str()}), m_path(path) {}

private:
  std::filesystem::path m_path;
};
