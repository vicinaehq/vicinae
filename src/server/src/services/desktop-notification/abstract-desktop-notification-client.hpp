#pragma once
#include <QString>
#include <optional>

class AbstractDesktopNotificationClient {
public:
  enum class Urgency { Low, Normal, High };

  struct Notification {
    QString title;
    QString body;
    std::optional<QString> iconPath;
    Urgency urgency = Urgency::Normal;
  };

  virtual bool send(const Notification &notification) = 0;
  virtual ~AbstractDesktopNotificationClient() = default;
};
