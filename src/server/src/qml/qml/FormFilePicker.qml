import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs

FocusScope {
    id: root
    implicitHeight: multiple ? multiLayout.implicitHeight : 36
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property bool multiple: false
    property bool directoriesOnly: false
    property bool readOnly: false
    property bool hasError: false
    property var selectedPaths: []

    signal pathsChanged(var paths)

    function forceActiveFocus() { focusItem.forceActiveFocus() }

    function _openDialog() {
        if (root.readOnly) return
        if (directoriesOnly) folderDialog.open()
        else fileDialog.open()
    }

    // --- Single mode ---

    Item {
        id: singleMode
        visible: !root.multiple
        anchors.fill: parent

        readonly property string _displayText: {
            if (!root.selectedPaths || root.selectedPaths.length === 0) return ""
            return root.selectedPaths[0] || ""
        }

        Rectangle {
            anchors.fill: parent
            radius: 8
            opacity: root.readOnly ? 0.5 : 1.0
            color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                           Theme.secondaryBackground.b, Config.windowOpacity)
            border.color: root.hasError ? Theme.inputBorderError
                          : focusItem.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
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
                    text: singleMode._displayText || (root.directoriesOnly ? "No directory selected" : "No file selected")
                    color: singleMode._displayText ? Theme.foreground : Theme.textPlaceholder
                    font.pointSize: Theme.regularFontSize
                    elide: Text.ElideMiddle
                }

                Rectangle {
                    visible: !root.readOnly && root.selectedPaths && root.selectedPaths.length > 0
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
                            root.pathsChanged([])
                            root.selectedPaths = []
                        }
                    }
                }
            }

            TapHandler {
                onTapped: {
                    focusItem.forceActiveFocus()
                    root._openDialog()
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
                color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                               Theme.secondaryBackground.b, Config.windowOpacity)
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

                    Rectangle {
                        visible: !root.readOnly
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.alignment: Qt.AlignVCenter
                        radius: 4
                        color: removeHover.hovered ? Theme.listItemHoverBg : "transparent"

                        ViciImage {
                            anchors.centerIn: parent
                            width: 10
                            height: 10
                            source: Img.builtin("xmark")
                        }

                        HoverHandler { id: removeHover }
                        TapHandler {
                            onTapped: {
                                const idx = index
                                let copy = []
                                for (let i = 0; i < root.selectedPaths.length; i++) {
                                    if (i !== idx) copy.push(root.selectedPaths[i])
                                }
                                root.pathsChanged(copy)
                                root.selectedPaths = copy
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            visible: !root.readOnly
            Layout.fillWidth: true
            implicitHeight: 32
            radius: 6
            color: addHover.hovered ? Theme.listItemHoverBg : "transparent"
            border.color: Theme.inputBorder
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: root.directoriesOnly ? "+ Add folder…" : "+ Add file…"
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
            }

            HoverHandler { id: addHover }
            TapHandler {
                onTapped: {
                    focusItem.forceActiveFocus()
                    root._openDialog()
                }
            }
        }
    }

    Item {
        id: focusItem
        width: 0
        height: 0
        focus: true
        activeFocusOnTab: false

        Keys.onReturnPressed: (event) => {
            if (typeof launcher !== "undefined")
                event.accepted = launcher.forwardKey(event.key, event.modifiers)
            if (!event.accepted)
                root._openDialog()
        }
        Keys.onSpacePressed: root._openDialog()
        Keys.onPressed: (event) => {
            if (typeof launcher !== "undefined")
                event.accepted = launcher.forwardKey(event.key, event.modifiers)
        }
    }

    function _appendUnique(existing, newPaths) {
        let merged = []
        for (let i = 0; i < existing.length; i++)
            merged.push(existing[i])
        for (let i = 0; i < newPaths.length; i++) {
            if (merged.indexOf(newPaths[i]) === -1)
                merged.push(newPaths[i])
        }
        return merged
    }

    FileDialog {
        id: fileDialog
        title: root.multiple ? "Select files" : "Select a file"
        fileMode: root.multiple ? FileDialog.OpenFiles : FileDialog.OpenFile
        onAccepted: {
            let paths = []
            for (let i = 0; i < selectedFiles.length; i++)
                paths.push(selectedFiles[i].toString().replace("file://", ""))

            if (root.multiple)
                paths = root._appendUnique(root.selectedPaths || [], paths)

            root.selectedPaths = paths
            root.pathsChanged(paths)
        }
    }

    FolderDialog {
        id: folderDialog
        title: "Select a directory"
        onAccepted: {
            const path = selectedFolder.toString().replace("file://", "")

            if (root.multiple) {
                const merged = root._appendUnique(root.selectedPaths || [], [path])
                root.selectedPaths = merged
                root.pathsChanged(merged)
            } else {
                root.selectedPaths = [path]
                root.pathsChanged([path])
            }
        }
    }
}
