#include "mac-file-chooser.hpp"
#import <AppKit/AppKit.h>
#include <qpointer.h>

MacFileChooser::MacFileChooser(QObject *parent) : AbstractFileChooser(parent) {}

MacFileChooser::~MacFileChooser() { close(); }

bool MacFileChooser::isAvailable() const { return true; }

bool MacFileChooser::open(const FileChooserOptions &options) {
  if (m_panel) return false;

  @autoreleasepool {
    bool const saveMode = options.mode == FileChooserMode::Save;
    NSSavePanel *panel = nil;

    if (saveMode) {
      panel = [NSSavePanel savePanel];
      panel.canCreateDirectories = YES;
      if (options.suggestedName) {
        NSString *name = [NSString stringWithUTF8String:options.suggestedName->c_str()];
        if (name) panel.nameFieldStringValue = name;
      }
    } else {
      NSOpenPanel *openPanel = [NSOpenPanel openPanel];
      openPanel.canChooseFiles = options.canChooseFiles;
      openPanel.canChooseDirectories = options.canChooseDirectories;
      openPanel.allowsMultipleSelection = options.allowMultipleSelection;
      openPanel.resolvesAliases = YES;
      panel = openPanel;
    }

    panel.showsHiddenFiles = options.showHiddenFiles;
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

      NSSavePanel *shown = (__bridge_transfer NSSavePanel *)self->m_panel;
      self->m_panel = nullptr;

      if (response != NSModalResponseOK) {
        emit self->rejected();
        return;
      }

      std::vector<std::filesystem::path> paths;

      if (saveMode) {
        if (shown.URL.fileURL) paths.emplace_back(shown.URL.path.UTF8String);
      } else {
        NSArray<NSURL *> *urls = ((NSOpenPanel *)shown).URLs;
        paths.reserve(urls.count);
        for (NSURL *url in urls) {
          if (url.fileURL) paths.emplace_back(url.path.UTF8String);
        }
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

  NSSavePanel *panel = (__bridge_transfer NSSavePanel *)m_panel;
  m_panel = nullptr;
  [panel cancel:nil];
}
