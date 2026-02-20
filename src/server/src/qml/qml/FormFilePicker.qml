import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs

FocusScope {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    property bool multiple: false
    property bool directoriesOnly: false
    property var selectedPaths: []

    signal pathsChanged(var paths)

    function forceActiveFocus() { focusItem.forceActiveFocus() }

    function _openDialog() {
        if (directoriesOnly) folderDialog.open()
        else fileDialog.open()
    }

    readonly property string _displayText: {
        if (!selectedPaths || selectedPaths.length === 0) return ""
        if (selectedPaths.length === 1) return selectedPaths[0]
        return selectedPaths.length + " files selected"
    }

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: Theme.secondaryBackground
        border.color: focusItem.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
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
                text: root._displayText || (root.directoriesOnly ? "No directory selected" : "No file selected")
                color: root._displayText ? Theme.foreground : Theme.textPlaceholder
                font.pointSize: Theme.regularFontSize
                elide: Text.ElideMiddle
            }

            Rectangle {
                visible: root.selectedPaths && root.selectedPaths.length > 0
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                Layout.alignment: Qt.AlignVCenter
                radius: 4
                color: clearHover.hovered ? Theme.listItemHoverBg : "transparent"

                ViciImage {
                    anchors.centerIn: parent
                    width: 10
                    height: 10
                    source: Img.builtin("xmark")
                }

                HoverHandler { id: clearHover }
                TapHandler {
                    onTapped: {
                        root.selectedPaths = []
                        root.pathsChanged([])
                    }
                }
            }
        }

        // Invisible focus target + click/key handler
        Item {
            id: focusItem
            anchors.fill: parent
            focus: true
            activeFocusOnTab: false

            TapHandler {
                onTapped: {
                    focusItem.forceActiveFocus()
                    root._openDialog()
                }
            }

            Keys.onReturnPressed: (event) => {
                if (event.modifiers !== Qt.NoModifier && typeof launcher !== "undefined") {
                    event.accepted = launcher.forwardKey(event.key, event.modifiers)
                } else {
                    root._openDialog()
                }
            }
            Keys.onSpacePressed: root._openDialog()
            Keys.onPressed: (event) => {
                if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
                    && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
                    && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta
                    && typeof launcher !== "undefined") {
                    event.accepted = launcher.forwardKey(event.key, event.modifiers)
                } else {
                    event.accepted = false
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: root.multiple ? "Select files" : "Select a file"
        fileMode: root.multiple ? FileDialog.OpenFiles : FileDialog.OpenFile
        onAccepted: {
            var paths = []
            for (var i = 0; i < selectedFiles.length; i++) {
                paths.push(selectedFiles[i].toString().replace("file://", ""))
            }
            root.selectedPaths = paths
            root.pathsChanged(paths)
        }
    }

    FolderDialog {
        id: folderDialog
        title: "Select a directory"
        onAccepted: {
            var path = selectedFolder.toString().replace("file://", "")
            root.selectedPaths = [path]
            root.pathsChanged([path])
        }
    }
}
