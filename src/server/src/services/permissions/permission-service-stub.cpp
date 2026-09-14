#include "services/permissions/macos-permission-service.hpp"

MacosPermissionService::MacosPermissionService(QObject *parent) : QObject(parent) {}
bool MacosPermissionService::supported() const { return false; }
void MacosPermissionService::setWatching(bool) {}
void MacosPermissionService::requestAccessibility() {}
void MacosPermissionService::requestFullDiskAccess() {}
void MacosPermissionService::requestNotifications() {}
void MacosPermissionService::refresh() {}
void MacosPermissionService::refreshNotifications() {}

namespace vicinae::permissions {
MicrophoneStatus microphoneStatus() { return MicrophoneStatus::Granted; }
void requestMicrophone(std::function<void(bool)> done) { done(true); }
void openMicrophoneSettings() {}
} // namespace vicinae::permissions
