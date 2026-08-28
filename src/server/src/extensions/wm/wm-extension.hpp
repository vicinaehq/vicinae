#pragma once
#include "command-database.hpp"
#include "service-registry.hpp"
#include "ui/image/url.hpp"
#include <qcontainerfwd.h>
#include <QCoreApplication>

class WindowManagementExtension : public BuiltinCommandRepository {
  QString id() const override;
  QString displayName() const override;
  ImageURL iconUrl() const override;

public:
  WindowManagementExtension(const ServiceRegistry &services);
};
