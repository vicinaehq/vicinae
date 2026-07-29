#include "macos-notification-client.hpp"
#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

namespace {

UNNotificationInterruptionLevel mapUrgency(AbstractDesktopNotificationClient::Urgency urgency) {
  using Urgency = AbstractDesktopNotificationClient::Urgency;
  switch (urgency) {
  case Urgency::Low:
    return UNNotificationInterruptionLevelPassive;
  case Urgency::High:
    return UNNotificationInterruptionLevelTimeSensitive;
  default:
    return UNNotificationInterruptionLevelActive;
  }
}

} // namespace

bool MacosNotificationClient::send(const Notification &n) {
  // UNUserNotificationCenter throws when the process does not run from an app bundle
  if (!NSBundle.mainBundle.bundleIdentifier) return false;

  UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
  content.title = n.title.toNSString();
  content.body = n.body.toNSString();
  content.interruptionLevel = mapUrgency(n.urgency);
  if (n.urgency != Urgency::Low) content.sound = UNNotificationSound.defaultSound;

  if (n.iconPath) {
    NSURL *url = [NSURL fileURLWithPath:n.iconPath->toNSString()];
    NSError *error = nil;
    UNNotificationAttachment *attachment = [UNNotificationAttachment attachmentWithIdentifier:@""
                                                                                          URL:url
                                                                                      options:nil
                                                                                        error:&error];
    if (attachment) content.attachments = @[ attachment ];
  }

  UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:NSUUID.UUID.UUIDString
                                                                        content:content
                                                                        trigger:nil];
  UNUserNotificationCenter *center = UNUserNotificationCenter.currentNotificationCenter;

  [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound)
                        completionHandler:^(BOOL granted, NSError *) {
                          if (granted) [center addNotificationRequest:request withCompletionHandler:nil];
                        }];

  return true;
}
