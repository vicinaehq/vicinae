#include "mac-file-chooser.hpp"
#import <AppKit/AppKit.h>
#include <qpointer.h>

MacFileChooser::MacFileChooser(QObject *parent) : AbstractFileChooser(parent) {}

MacFileChooser::~MacFileChooser() { close(); }

bool MacFileChooser::isAvailable() const { return true; }

bool MacFileChooser::open(const FileChooserOptions &options) {
  if (m_panel) return false;

  @autoreleasepool {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    panel.canChooseFiles = options.canChooseFiles;
    panel.canChooseDirectories = options.canChooseDirectories;
    panel.allowsMultipleSelection = options.allowMultipleSelection;
    panel.showsHiddenFiles = options.showHiddenFiles;
    panel.resolvesAliases = YES;
    // above the launcher's floating panel level
    panel.level = NSModalPanelWindowLevel;

    if (options.currentFolder) {
      NSString *path = [NSString stringWithUTF8String:options.currentFolder->c_str()];
      if (path) panel.directoryURL = [NSURL fileURLWithPath:path isDirectory:YES];
    }

    // close() clears m_panel before dismissing, keeping the handler quiet
    m_panel = (__bridge_retained void *)panel;

    QPointer<MacFileChooser> self(this);

    [NSApp activateIgnoringOtherApps:YES];
    [panel beginWithCompletionHandler:^(NSModalResponse response) {
      if (!self || !self->m_panel) return;

      NSOpenPanel *shown = (__bridge_transfer NSOpenPanel *)self->m_panel;
      self->m_panel = nullptr;

      if (response != NSModalResponseOK) {
        emit self->rejected();
        return;
      }

      std::vector<std::filesystem::path> paths;
      paths.reserve(shown.URLs.count);
      for (NSURL *url in shown.URLs) {
        if (url.fileURL) paths.emplace_back(url.path.UTF8String);
      }

      if (paths.empty()) {
        emit self->rejected();
        return;
      }

      emit self->filesChosen(paths);
    }];
    [panel makeKeyAndOrderFront:nil];
  }

  return true;
}

void MacFileChooser::close() {
  if (!m_panel) return;

  NSOpenPanel *panel = (__bridge_transfer NSOpenPanel *)m_panel;
  m_panel = nullptr;
  [panel cancel:nil];
}
