pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Dialogs
import Vicinae

Item {
    id: root
    property bool multiple: false
    property bool canChooseFiles: true
    property bool canChooseDirectories: false
    property bool _waiting: false
    signal accepted(var paths)
    signal closed
    width: 0
    height: 0

    function open() {
        if (FileChooser.active)
            return;
        _waiting = true;
        if (!FileChooser.openDialog(canChooseFiles, canChooseDirectories, multiple)) {
            if (canChooseDirectories && !canChooseFiles)
                folders.open();
            else
                files.open();
        }
    }
    function acceptUrls(urls) {
        root.accepted(Array.from(urls, url => FileChooser.toLocalPath(url)));
        FileChooser.notifyFallbackDone();
    }
    Connections {
        target: FileChooser
        enabled: root._waiting
        function onFilesSelected(paths) {
            root.accepted(Array.from(paths));
        }
        function onDialogClosed() {
            root._waiting = false;
            root.closed();
        }
    }
    FileDialog {
        id: files
        fileMode: root.multiple ? FileDialog.OpenFiles : FileDialog.OpenFile
        title: root.multiple ? qsTr("Select files") : qsTr("Select a file")
        onAccepted: root.acceptUrls(selectedFiles)
        onRejected: FileChooser.notifyFallbackDone()
    }
    FolderDialog {
        id: folders
        title: qsTr("Select a directory")
        onAccepted: root.acceptUrls([selectedFolder])
        onRejected: FileChooser.notifyFallbackDone()
    }
}
