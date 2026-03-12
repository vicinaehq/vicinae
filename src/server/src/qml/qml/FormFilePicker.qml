import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

FocusScope {
    id: root
    implicitHeight: multiple ? multiLayout.implicitHeight : 36
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property bool multiple: false
    property bool canChooseDirectories: false
    property bool canChooseFiles: true
    property bool readOnly: false
    property bool hasError: false
    property var selectedPaths: []

    signal pathsChanged(var paths)
    signal openRequested
    signal fallbackDialogClosed

    readonly property bool _directoriesOnly: canChooseDirectories && !canChooseFiles

    function forceActiveFocus() {
        focusItem.forceActiveFocus();
    }

    function _openDialog() {
        if (root.readOnly)
            return;
        root.openRequested();
    }

    function openFallbackDialog() {
        if (root._directoriesOnly)
            _fallbackFolderDialog.open();
        else {
            _fallbackFileDialog.fileMode = root.multiple ? FileDialog.OpenFiles : FileDialog.OpenFile;
            _fallbackFileDialog.open();
        }
    }

    function _handleFallbackResult(urls) {
        let newPaths = [];
        for (let i = 0; i < urls.length; i++)
            newPaths.push(urls[i].toString().replace("file://", ""));
        if (root.multiple && root.selectedPaths) {
            let merged = [];
            for (let i = 0; i < root.selectedPaths.length; i++)
                merged.push(root.selectedPaths[i]);
            for (let i = 0; i < newPaths.length; i++) {
                if (merged.indexOf(newPaths[i]) < 0)
                    merged.push(newPaths[i]);
            }
            newPaths = merged;
        }
        root.pathsChanged(newPaths);
    }

    FileDialog {
        id: _fallbackFileDialog
        title: root.multiple ? "Select files" : "Select a file"
        onAccepted: {
            root._handleFallbackResult(selectedFiles);
            root.fallbackDialogClosed();
        }
        onRejected: root.fallbackDialogClosed()
    }

    FolderDialog {
        id: _fallbackFolderDialog
        title: "Select a directory"
        onAccepted: {
            root._handleFallbackResult([selectedFolder]);
            root.fallbackDialogClosed();
        }
        onRejected: root.fallbackDialogClosed()
    }

    // --- Single mode ---

    Item {
        id: singleMode
        visible: !root.multiple
        anchors.fill: parent

        readonly property string _displayText: {
            if (!root.selectedPaths || root.selectedPaths.length === 0)
                return "";
            return root.selectedPaths[0] || "";
        }

        Rectangle {
            anchors.fill: parent
            radius: 8
            opacity: root.readOnly ? 0.5 : 1.0
            color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, Config.windowOpacity)
            border.color: root.hasError ? Theme.inputBorderError : focusItem.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 6

                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    text: singleMode._displayText || (root._directoriesOnly ? "No directory selected" : "No file selected")
                    color: singleMode._displayText ? Theme.foreground : Theme.textPlaceholder
                    font.pointSize: Theme.regularFontSize
                    elide: Text.ElideMiddle
                }

                ViciButton {
                    visible: !root.readOnly && root.selectedPaths && root.selectedPaths.length > 0
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignVCenter
                    radius: 4
                    icon: "xmark"
                    iconSize: 10
                    variant: "ghost"
                    onClicked: root.pathsChanged([])
                }
            }

            TapHandler {
                onTapped: {
                    focusItem.forceActiveFocus();
                    root._openDialog();
                }
            }
        }
    }

    // --- Multi mode ---

    ColumnLayout {
        id: multiLayout
        visible: root.multiple
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 6
        opacity: root.readOnly ? 0.5 : 1.0

        Repeater {
            model: root.multiple ? root.selectedPaths : []

            Rectangle {
                required property int index
                required property var modelData

                Layout.fillWidth: true
                implicitHeight: 32
                radius: 6
                color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, Config.windowOpacity)
                border.color: Theme.inputBorder
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 6
                    spacing: 6

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Text.AlignVCenter
                        text: modelData
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        elide: Text.ElideMiddle
                    }

                    ViciButton {
                        visible: !root.readOnly
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.alignment: Qt.AlignVCenter
                        radius: 4
                        icon: "xmark"
                        iconSize: 10
                        variant: "ghost"
                        onClicked: {
                            const idx = index;
                            let copy = [];
                            for (let i = 0; i < root.selectedPaths.length; i++) {
                                if (i !== idx)
                                    copy.push(root.selectedPaths[i]);
                            }
                            root.pathsChanged(copy);
                        }
                    }
                }
            }
        }

        ViciButton {
            visible: !root.readOnly
            Layout.fillWidth: true
            implicitHeight: 32
            radius: 6
            text: root._directoriesOnly ? "+ Add folder…" : "+ Add file…"
            foreground: Theme.textMuted
            variant: "ghost"
            bordered: true
            onClicked: {
                focusItem.forceActiveFocus();
                root._openDialog();
            }
        }
    }

    Item {
        id: focusItem
        width: 0
        height: 0
        focus: true
        activeFocusOnTab: false

        Keys.onReturnPressed: root._openDialog()
        Keys.onSpacePressed: root._openDialog()
    }
}
