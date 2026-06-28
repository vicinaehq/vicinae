#include "macos-power-manager.hpp"
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>

extern "C" void SACLockScreenImmediate(void);

namespace {

bool sendSystemAppleEvent(AEEventID event) {
  NSData *const targetData = [@"com.apple.loginwindow" dataUsingEncoding:NSUTF8StringEncoding];

  AEAddressDesc target = {typeNull, nullptr};
  if (AECreateDesc(typeApplicationBundleID, targetData.bytes, targetData.length, &target) != noErr) {
    return false;
  }

  AppleEvent request = {typeNull, nullptr};
  OSStatus status = AECreateAppleEvent(kCoreEventClass, event, &target, kAutoGenerateReturnID,
                                       kAnyTransactionID, &request);
  AEDisposeDesc(&target);
  if (status != noErr) { return false; }

  AppleEvent reply = {typeNull, nullptr};
  status = AESendMessage(&request, &reply, kAENormalPriority, kAEDefaultTimeout);
  AEDisposeDesc(&request);
  AEDisposeDesc(&reply);

  return status == noErr;
}

} // namespace

bool MacosPowerManager::powerOff() { return sendSystemAppleEvent(kAEShutDown); }
bool MacosPowerManager::reboot() { return sendSystemAppleEvent(kAERestart); }
bool MacosPowerManager::sleep() const { return sendSystemAppleEvent(kAESleep); }
bool MacosPowerManager::logout() { return sendSystemAppleEvent(kAEReallyLogOut); }

bool MacosPowerManager::lock() {
  SACLockScreenImmediate();
  return true;
}

bool MacosPowerManager::canPowerOff() const { return true; }
bool MacosPowerManager::canReboot() const { return true; }
bool MacosPowerManager::canSleep() const { return true; }
bool MacosPowerManager::canLock() const { return true; }
bool MacosPowerManager::canLogOut() const { return true; }

QString MacosPowerManager::id() const { return "macos"; }
