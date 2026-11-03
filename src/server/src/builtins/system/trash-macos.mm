#include "trash.hpp"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

namespace SystemTrash {

bool emptyTrash(std::string &error) {
  @autoreleasepool {
    NSString *scriptSource = @"tell application \"Finder\"\n"
                              "  set warns to warns before emptying\n"
                              "  set warns before emptying to false\n"
                              "  empty trash\n"
                              "  set warns before emptying to warns\n"
                              "end tell";
    NSAppleScript *script = [[NSAppleScript alloc] initWithSource:scriptSource];
    NSDictionary *errorInfo = nil;
    [script executeAndReturnError:&errorInfo];

    if (errorInfo) {
      NSAppleScript *fallbackScript =
          [[NSAppleScript alloc] initWithSource:@"tell application \"Finder\" to empty the trash"];
      NSDictionary *fallbackErrorInfo = nil;
      [fallbackScript executeAndReturnError:&fallbackErrorInfo];

      if (fallbackErrorInfo) {
        NSString *msg = fallbackErrorInfo[NSAppleScriptErrorMessage];
        error = msg ? [msg UTF8String] : "Failed to empty trash";
        return false;
      }
    }
    return true;
  }
}

bool openTrash(std::string &error) {
  @autoreleasepool {
    NSString *trashPath = [@"~/.Trash" stringByExpandingTildeInPath];
    NSURL *trashUrl = [NSURL fileURLWithPath:trashPath];
    if (![[NSWorkspace sharedWorkspace] openURL:trashUrl]) {
      error = "Failed to open trash in Finder";
      return false;
    }
    return true;
  }
}

} // namespace SystemTrash
