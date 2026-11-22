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
    readonly property list<Item> titlebarControls: [titleInput, searchButton, newButton, editButton, splitButton, previewButton, moreButton]
    readonly property bool splitView: NotesWindowHost.viewMode === NotesWindowHost.Split
    readonly property bool previewOnly: NotesWindowHost.viewMode === NotesWindowHost.Preview
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
            readOnly: NotesWindowHost.readOnly || NotesWindowHost.loading
            Layout.fillWidth: true
            Layout.minimumWidth: 60
            Layout.maximumWidth: Math.max(120, implicitWidth)
            Layout.fillHeight: true
            padding: 6
            Binding on text {
                value: NotesWindowHost.title
                when: !titleInput.activeFocus
                restoreMode: Binding.RestoreNone
            }
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
            accessibleName: qsTr("Find in Note")
            activeFocusOnTab: true
            onClicked: root.openFind()
            ToolTip.visible: hovered
            ToolTip.delay: 600
            ToolTip.text: accessibleName
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
            implicitWidth: 122
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
                    iconSource: Img.icon(BuiltinIcon.Pencil).withFillColor(NotesWindowHost.viewMode === NotesWindowHost.Edit ? Theme.foreground : Theme.textMuted)
                    accessibleName: qsTr("Edit note")
                    Accessible.role: Accessible.RadioButton
                    Accessible.checkable: true
                    Accessible.checked: NotesWindowHost.viewMode === NotesWindowHost.Edit
                    Accessible.onToggleAction: editButton.clicked()
                    activeFocusOnTab: true
                    variant: NotesWindowHost.viewMode === NotesWindowHost.Edit ? "secondary" : "ghost"
                    ToolTip.visible: hovered
                    ToolTip.delay: 600
                    ToolTip.text: accessibleName
                    onClicked: NotesWindowHost.viewMode = NotesWindowHost.Edit
                }
                ViciButton {
                    id: splitButton
                    width: 38
                    height: root.toolbarButtonSize - 4
                    radius: 5
                    iconSource: Img.icon(BuiltinIcon.AppWindowSidebarRight).withFillColor(root.splitView ? Theme.foreground : Theme.textMuted)
                    accessibleName: qsTr("Side by side")
                    Accessible.role: Accessible.RadioButton
                    Accessible.checkable: true
                    Accessible.checked: root.splitView
                    Accessible.onToggleAction: splitButton.clicked()
                    activeFocusOnTab: true
                    variant: root.splitView ? "secondary" : "ghost"
                    ToolTip.visible: hovered
                    ToolTip.delay: 600
                    ToolTip.text: accessibleName
                    onClicked: NotesWindowHost.viewMode = NotesWindowHost.Split
                }
                ViciButton {
                    id: previewButton
                    width: 38
                    height: root.toolbarButtonSize - 4
                    radius: 5
                    iconSource: Img.icon(BuiltinIcon.Eye).withFillColor(root.previewOnly ? Theme.foreground : Theme.textMuted)
                    accessibleName: qsTr("Preview note")
                    Accessible.role: Accessible.RadioButton
                    Accessible.checkable: true
                    Accessible.checked: root.previewOnly
                    Accessible.onToggleAction: previewButton.clicked()
                    activeFocusOnTab: true
                    variant: root.previewOnly ? "secondary" : "ghost"
                    ToolTip.visible: hovered
                    ToolTip.delay: 600
                    ToolTip.text: accessibleName
                    onClicked: NotesWindowHost.viewMode = NotesWindowHost.Preview
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
                    text: qsTr("Search Notes…")
                    onTriggered: NotesWindowHost.search(moreButton)
                }
                ViciMenuItem {
                    text: qsTr("Save")
                    enabled: !NotesWindowHost.readOnly
                    onTriggered: NotesWindowHost.save()
                }
                ViciMenuItem {
                    text: qsTr("Save a Copy")
                    onTriggered: NotesWindowHost.saveCopy()
                }
                ViciMenuItem {
                    text: qsTr("Delete Note…")
                    enabled: !NotesWindowHost.readOnly
                    onTriggered: {
                        noteActionDialog.reloadFile = false;
                        noteActionDialog.open();
                    }
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
                implicitHeight: errorContent.implicitHeight + 24
                color: Config.withAlpha(Theme.danger, 0.1)
                ColumnLayout {
                    id: errorContent
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 12
                    Text {
                        Layout.fillWidth: true
                        text: NotesWindowHost.error
                        wrapMode: Text.Wrap
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                    }
                    RowLayout {
                        ViciButton {
                            text: qsTr("Save a Copy")
                            onClicked: NotesWindowHost.saveCopy()
                        }
                        ViciButton {
                            text: qsTr("Reload…")
                            onClicked: {
                                noteActionDialog.reloadFile = true;
                                noteActionDialog.open();
                            }
                        }
                        ViciButton {
                            text: qsTr("Settings")
                            onClicked: NotesWindowHost.openSettings()
                        }
                    }
                }
            }

            SplitView {
                id: panes
                Layout.fillWidth: true
                Layout.fillHeight: true
                enabled: !NotesWindowHost.loading
                orientation: Qt.Horizontal
                handle: Rectangle {
                    implicitWidth: 9
                    color: "transparent"
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 1
                        height: parent.height
                        color: parent.SplitHandle.pressed || parent.SplitHandle.hovered ? Theme.inputBorderFocus : Theme.divider
                    }
                }
                Item {
                    visible: !root.previewOnly
                    SplitView.minimumWidth: 220
                    SplitView.preferredWidth: panes.width / 2
                    SplitView.fillWidth: !root.splitView
                    MarkdownEditor {
                        id: editor
                        anchors.fill: parent
                        anchors.bottomMargin: findBar.visible ? findBar.height + 8 : 0
                        document: NotesWindowHost.document
                        readOnly: NotesWindowHost.readOnly || NotesWindowHost.loading
                        foreground: Theme.foreground
                        mutedForeground: Theme.textPlaceholder
                        selectionColor: Theme.textSelectionBg
                        selectedTextColor: Theme.textSelectionFg
                        toolbarBackground: Theme.popoverBackground
                        toolbarBorder: Theme.popoverBorder
                        toolbarHover: Theme.listItemHoverBg
                        formattingIcons: ({
                                bold: "qrc:/icons/bold.svg",
                                italic: "qrc:/icons/italics.svg",
                                strike: "qrc:/icons/strike-through.svg",
                                code: "qrc:/icons/code.svg",
                                link: "qrc:/icons/link.svg",
                                heading: "qrc:/icons/heading.svg",
                                codeBlock: "qrc:/icons/code-block.svg",
                                bullets: "qrc:/icons/bullet-points.svg",
                                numbered: "qrc:/icons/numbered-list.svg",
                                tasks: "qrc:/icons/check-list.svg",
                                quote: "qrc:/icons/quote-block.svg"
                            })
                        fontFamily: Theme.monoFontFamily
                        fontSize: Theme.regularFontSize
                        contentPadding: root.splitView ? 24 : Math.max(32, (width - 760) / 2)
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
                }
                MarkdownView {
                    visible: root.splitView || root.previewOnly
                    SplitView.minimumWidth: 220
                    SplitView.fillWidth: true
                    model: NotesWindowHost.previewModel
                    fontSize: editor.fontSize
                    headingScale: 0.5
                    codeFontSize: editor.fontSize
                    contentPadding: root.splitView ? 24 : Math.max(32, (width - 760) / 2)
                    topPadding: 20
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 14
                spacing: 6
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
        id: noteActionDialog
        property bool reloadFile: false
        parent: Overlay.overlay
        width: Math.min(parent.width - 40, 400)
        padding: 24
        contentItem: ColumnLayout {
            spacing: 16
            Text {
                Layout.fillWidth: true
                text: noteActionDialog.reloadFile ? qsTr("Reload Note?") : qsTr("Delete Note?")
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: noteActionDialog.reloadFile ? qsTr("Discard your unsaved changes and reload the file?") : qsTr("This note will be permanently deleted.")
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
                    onClicked: noteActionDialog.close()
                }
                ViciButton {
                    Layout.fillWidth: true
                    text: noteActionDialog.reloadFile ? qsTr("Reload") : qsTr("Delete")
                    foreground: Theme.danger
                    variant: "tinted"
                    activeFocusOnTab: true
                    onClicked: {
                        noteActionDialog.close();
                        if (noteActionDialog.reloadFile)
                            NotesWindowHost.reload();
                        else
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
        if (root.previewOnly)
            NotesWindowHost.viewMode = NotesWindowHost.Edit;
        findBar.visible = true;
        findInput.forceActiveFocus();
        findInput.selectAll();
    }
    Shortcut {
        sequences: [StandardKey.Close]
        onActivated: root.close()
    }
    Shortcut {
        sequences: [StandardKey.Quit]
        onActivated: Qt.quit()
    }
    Shortcut {
        sequences: [StandardKey.Preferences]
        onActivated: NotesWindowHost.openSettings()
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
        onActivated: NotesWindowHost.search(moreButton)
    }
    Shortcut {
        sequence: "Ctrl+Shift+P"
        onActivated: NotesWindowHost.viewMode = root.previewOnly ? NotesWindowHost.Edit : NotesWindowHost.Preview
    }
    Connections {
        target: NotesWindowHost
        function onViewModeChanged() {
            if (root.previewOnly)
                findBar.visible = false;
        }
        function onFocusRequested() {
            if (!root.previewOnly)
                editor.focusEditor();
        }
    }
}
