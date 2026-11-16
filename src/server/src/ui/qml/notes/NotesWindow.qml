pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae
import Vicinae.Editor

Window {
    id: root
    property bool nativeChrome: false
    property bool translucentBackground: false
    property real titlebarRightInset: 0
    property real headerHeight: 48
    property real toolbarCenterY: headerHeight / 2
    property real toolbarLeftInset: 12
    readonly property real toolbarButtonSize: 30
    readonly property list<Item> titlebarControls: [titleInput, searchButton, newButton, editButton, previewButton, moreButton]
    width: 800
    height: 760
    minimumWidth: 480
    minimumHeight: 400
    color: "transparent"
    objectName: "vicinae-notes"
    // Keep the product prefix stable for window-manager rules, including across locales.
    title: "Vicinae Notes — %1".arg(NotesWindowHost.title || qsTr("Untitled Note"))
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    WindowMaterial.enabled: Platform.supports("windowMaterial")
    onClosing: event => event.accepted = NotesWindowHost.requestClose()

    Rectangle {
        width: parent.width
        height: root.headerHeight
        color: root.translucentBackground ? Config.withAlpha(Theme.background, 0.58) : Theme.background
        DragHandler {
            target: null
            onActiveChanged: if (active)
                root.startSystemMove()
        }
    }
    RowLayout {
        x: root.toolbarLeftInset
        y: Math.round(root.toolbarCenterY - height / 2)
        width: parent.width - x - 12 - root.titlebarRightInset
        height: root.toolbarButtonSize
        spacing: 6
        TextField {
            id: titleInput
            Layout.fillWidth: true
            Layout.minimumWidth: 60
            Layout.maximumWidth: Math.max(120, implicitWidth)
            Layout.fillHeight: true
            padding: 6
            text: NotesWindowHost.title
            placeholderText: qsTr("Untitled Note")
            placeholderTextColor: Theme.textPlaceholder
            color: Theme.foreground
            selectionColor: Theme.textSelectionBg
            selectedTextColor: Theme.textSelectionFg
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            font.weight: Font.DemiBold
            verticalAlignment: TextInput.AlignVCenter
            selectByMouse: true
            clip: true
            enabled: !NotesWindowHost.loading
            background: Rectangle {
                radius: 4
                color: titleInput.activeFocus ? Config.withAlpha(Theme.foreground, 0.04) : "transparent"
                border.width: titleInput.activeFocus || titleInput.hovered ? 1 : 0
                border.color: titleInput.activeFocus ? Theme.inputBorderFocus : Theme.divider
            }
            Accessible.name: qsTr("Note title")
            onTextEdited: NotesWindowHost.title = text
            onAccepted: editor.focusEditor()
            KeyNavigation.tab: editor
            ToolTip.visible: titleInput.hovered && !titleInput.activeFocus && titleInput.contentWidth > titleInput.width - titleInput.leftPadding - titleInput.rightPadding
            ToolTip.delay: 600
            ToolTip.text: text
        }
        Item {
            Layout.fillWidth: true
        }
        ViciButton {
            id: searchButton
            implicitHeight: root.toolbarButtonSize
            iconSource: Img.icon(BuiltinIcon.MagnifyingGlass)
            accessibleName: qsTr("Search Notes")
            activeFocusOnTab: true
            onClicked: NotesWindowHost.search(searchButton)
            ToolTip.visible: hovered
            ToolTip.delay: 600
            ToolTip.text: qsTr("Search Notes")
        }
        ViciButton {
            id: newButton
            implicitHeight: root.toolbarButtonSize
            iconSource: Img.icon(BuiltinIcon.Plus)
            accessibleName: qsTr("Create Note")
            activeFocusOnTab: true
            onClicked: NotesWindowHost.newNote()
            ToolTip.visible: hovered
            ToolTip.delay: 600
            ToolTip.text: qsTr("Create Note")
        }
        Rectangle {
            implicitWidth: 1
            implicitHeight: 16
            color: Theme.divider
        }
        Rectangle {
            implicitWidth: 82
            implicitHeight: root.toolbarButtonSize
            radius: 7
            color: Config.withAlpha(Theme.foreground, 0.025)
            border.color: Theme.divider
            enabled: !NotesWindowHost.loading
            Row {
                anchors.centerIn: parent
                spacing: 2
                ViciButton {
                    id: editButton
                    width: 38
                    height: root.toolbarButtonSize - 4
                    radius: 5
                    iconSource: Img.icon(BuiltinIcon.Pencil).withFillColor(NotesWindowHost.preview ? Theme.textMuted : Theme.foreground)
                    accessibleName: qsTr("Edit note")
                    Accessible.role: Accessible.RadioButton
                    Accessible.checkable: true
                    Accessible.checked: !NotesWindowHost.preview
                    Accessible.onToggleAction: editButton.clicked()
                    activeFocusOnTab: true
                    variant: NotesWindowHost.preview ? "ghost" : "secondary"
                    ToolTip.visible: hovered
                    ToolTip.delay: 600
                    ToolTip.text: accessibleName
                    onClicked: NotesWindowHost.preview = false
                }
                ViciButton {
                    id: previewButton
                    width: 38
                    height: root.toolbarButtonSize - 4
                    radius: 5
                    iconSource: Img.icon(BuiltinIcon.Eye).withFillColor(NotesWindowHost.preview ? Theme.foreground : Theme.textMuted)
                    accessibleName: qsTr("Preview note")
                    Accessible.role: Accessible.RadioButton
                    Accessible.checkable: true
                    Accessible.checked: NotesWindowHost.preview
                    Accessible.onToggleAction: previewButton.clicked()
                    activeFocusOnTab: true
                    variant: NotesWindowHost.preview ? "secondary" : "ghost"
                    ToolTip.visible: hovered
                    ToolTip.delay: 600
                    ToolTip.text: accessibleName
                    onClicked: NotesWindowHost.preview = true
                }
            }
        }
        ViciButton {
            id: moreButton
            implicitHeight: root.toolbarButtonSize
            enabled: !NotesWindowHost.loading
            iconSource: Img.icon(BuiltinIcon.Ellipsis)
            accessibleName: qsTr("Note Actions")
            activeFocusOnTab: true
            onClicked: actions.popup()
            ViciMenu {
                id: actions
                ViciMenuItem {
                    text: qsTr("Find in Note…")
                    onTriggered: root.openFind()
                }
                ViciMenuItem {
                    text: qsTr("Save")
                    onTriggered: NotesWindowHost.save()
                }
                ViciMenuItem {
                    text: qsTr("Delete Note…")
                    onTriggered: deleteDialog.open()
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.headerHeight
        color: root.translucentBackground ? Config.withAlpha(Theme.background, 0.84) : Theme.background

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Theme.divider
                opacity: 0.55
            }
            Rectangle {
                visible: NotesWindowHost.error.length > 0
                Layout.fillWidth: true
                implicitHeight: errorText.implicitHeight + 24
                color: Config.withAlpha(Theme.danger, 0.1)
                Text {
                    id: errorText
                    anchors.fill: parent
                    anchors.margins: 12
                    text: NotesWindowHost.error
                    wrapMode: Text.Wrap
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                enabled: !NotesWindowHost.loading
                MarkdownEditor {
                    id: editor
                    anchors.fill: parent
                    anchors.bottomMargin: findBar.visible ? findBar.height + 8 : 0
                    visible: !NotesWindowHost.preview
                    document: NotesWindowHost.document
                    foreground: Theme.foreground
                    mutedForeground: Theme.textPlaceholder
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    fontFamily: Theme.monoFontFamily
                    topPadding: 20
                    ViciWheelHandler {
                        target: editor.flickable
                    }
                }
                Rectangle {
                    id: findBar
                    visible: false
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: editor.contentPadding
                    width: parent.width - editor.contentPadding * 2
                    height: 42
                    radius: 8
                    color: Theme.secondaryBackground
                    border.color: Theme.divider
                    Keys.onShortcutOverride: event => event.accepted = event.key === Qt.Key_Escape
                    Keys.onEscapePressed: root.closeFind()
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 6
                        spacing: 2
                        TextField {
                            id: findInput
                            Layout.fillWidth: true
                            Layout.minimumWidth: 60
                            Layout.fillHeight: true
                            placeholderText: qsTr("Find in note…")
                            Accessible.name: placeholderText
                            placeholderTextColor: Theme.textMuted
                            color: Theme.foreground
                            selectionColor: Theme.textSelectionBg
                            selectedTextColor: Theme.textSelectionFg
                            font.family: Theme.fontFamily
                            font.pointSize: Theme.regularFontSize
                            padding: 6
                            background: null
                            onTextEdited: editor.find(text, false, true)
                            Keys.onReturnPressed: event => editor.find(text, !!(event.modifiers & Qt.ShiftModifier))
                            Keys.onEnterPressed: event => editor.find(text, !!(event.modifiers & Qt.ShiftModifier))
                        }
                        ViciButton {
                            implicitWidth: 26
                            implicitHeight: 28
                            iconSource: Img.icon(BuiltinIcon.ChevronUp).withFillColor(Theme.textMuted)
                            accessibleName: qsTr("Previous match")
                            activeFocusOnTab: true
                            ToolTip.visible: hovered
                            ToolTip.delay: 600
                            ToolTip.text: accessibleName
                            onClicked: editor.find(findInput.text, true)
                        }
                        ViciButton {
                            implicitWidth: 26
                            implicitHeight: 28
                            iconSource: Img.icon(BuiltinIcon.ChevronDown).withFillColor(Theme.textMuted)
                            accessibleName: qsTr("Next match")
                            activeFocusOnTab: true
                            ToolTip.visible: hovered
                            ToolTip.delay: 600
                            ToolTip.text: accessibleName
                            onClicked: editor.find(findInput.text, false)
                        }
                        ViciButton {
                            implicitWidth: 26
                            implicitHeight: 28
                            iconSource: Img.icon(BuiltinIcon.Xmark).withFillColor(Theme.textMuted)
                            accessibleName: qsTr("Close find")
                            activeFocusOnTab: true
                            onClicked: root.closeFind()
                        }
                    }
                }
                MarkdownView {
                    anchors.fill: parent
                    visible: NotesWindowHost.preview
                    model: NotesWindowHost.previewModel
                    contentPadding: editor.contentPadding
                    topPadding: 20
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 14
                Text {
                    text: NotesWindowHost.status
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "Markdown"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                }
            }
        }
    }

    ViciModal {
        id: deleteDialog
        parent: Overlay.overlay
        width: Math.min(parent.width - 40, 400)
        padding: 24
        contentItem: ColumnLayout {
            spacing: 16
            Text {
                Layout.fillWidth: true
                text: qsTr("Delete Note?")
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: qsTr("This note will be permanently deleted.")
                wrapMode: Text.Wrap
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
            }
            RowLayout {
                ViciButton {
                    Layout.fillWidth: true
                    text: qsTr("Cancel")
                    variant: "secondary"
                    focus: true
                    activeFocusOnTab: true
                    onClicked: deleteDialog.close()
                }
                ViciButton {
                    Layout.fillWidth: true
                    text: qsTr("Delete")
                    foreground: Theme.danger
                    variant: "tinted"
                    activeFocusOnTab: true
                    onClicked: {
                        deleteDialog.close();
                        NotesWindowHost.deleteCurrent();
                    }
                }
            }
        }
    }
    function closeFind() {
        findBar.visible = false;
        editor.focusEditor();
    }
    function openFind() {
        NotesWindowHost.preview = false;
        findBar.visible = true;
        findInput.forceActiveFocus();
        findInput.selectAll();
    }
    Shortcut {
        sequences: [StandardKey.Close]
        onActivated: root.close()
    }
    Shortcut {
        sequences: [StandardKey.New]
        onActivated: NotesWindowHost.newNote()
    }
    Shortcut {
        sequences: [StandardKey.Save]
        onActivated: NotesWindowHost.save()
    }
    Shortcut {
        sequences: [StandardKey.Find]
        onActivated: root.openFind()
    }
    Shortcut {
        sequence: "Ctrl+Shift+F"
        onActivated: NotesWindowHost.search(searchButton)
    }
    Shortcut {
        sequence: "Ctrl+Shift+P"
        onActivated: NotesWindowHost.preview = !NotesWindowHost.preview
    }
    Connections {
        target: NotesWindowHost
        function onPreviewChanged() {
            if (NotesWindowHost.preview)
                findBar.visible = false;
        }
        function onFocusRequested() {
            if (!NotesWindowHost.preview)
                editor.focusEditor();
        }
    }
}
