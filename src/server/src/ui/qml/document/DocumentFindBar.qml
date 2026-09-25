pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Rectangle {
    id: root
    required property DocumentSearch search
    property string placeholder: qsTr("Find in document…")
    signal closed
    visible: false
    implicitWidth: 360
    implicitHeight: 42
    radius: 8
    color: Theme.secondaryBackground
    border.color: Theme.divider

    function open() {
        visible = true;
        input.forceActiveFocus();
        input.selectAll();
    }

    function close() {
        visible = false;
        search.query = "";
        input.text = "";
        closed();
    }

    Keys.onShortcutOverride: event => event.accepted = event.key === Qt.Key_Escape
    Keys.onEscapePressed: root.close()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 2

        TextField {
            id: input
            Layout.fillWidth: true
            Layout.minimumWidth: 60
            Layout.fillHeight: true
            placeholderText: root.placeholder
            Accessible.name: root.placeholder
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
            placeholderTextColor: Theme.textMuted
            selectionColor: Theme.textSelectionBg
            selectedTextColor: Theme.textSelectionFg
            padding: 6
            background: null
            onTextEdited: root.search.query = text
            Keys.onReturnPressed: event => {
                if (event.modifiers & Qt.ShiftModifier)
                    root.search.previous();
                else
                    root.search.next();
            }
            Keys.onEnterPressed: event => {
                if (event.modifiers & Qt.ShiftModifier)
                    root.search.previous();
                else
                    root.search.next();
            }
        }

        Text {
            text: root.search.resultQuery.length === 0 ? (root.search.busy ? "…" : "") : root.search.count > 0 ? qsTr("%1 / %2").arg(root.search.currentIndex + 1).arg(root.search.count) : qsTr("No matches")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            Layout.rightMargin: 6
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        ViciButton {
            implicitWidth: 26
            implicitHeight: 28
            enabled: root.search.count > 0
            iconSource: Img.icon(BuiltinIcon.ChevronUp).withFillColor(Theme.textMuted)
            accessibleName: qsTr("Previous match")
            activeFocusOnTab: true
            ToolTip.visible: hovered
            ToolTip.text: accessibleName
            ToolTip.delay: 600
            onClicked: root.search.previous()
        }
        ViciButton {
            implicitWidth: 26
            implicitHeight: 28
            enabled: root.search.count > 0
            iconSource: Img.icon(BuiltinIcon.ChevronDown).withFillColor(Theme.textMuted)
            accessibleName: qsTr("Next match")
            activeFocusOnTab: true
            ToolTip.visible: hovered
            ToolTip.text: accessibleName
            ToolTip.delay: 600
            onClicked: root.search.next()
        }
        ViciButton {
            implicitWidth: 26
            implicitHeight: 28
            iconSource: Img.icon(BuiltinIcon.Xmark).withFillColor(Theme.textMuted)
            accessibleName: qsTr("Close find")
            activeFocusOnTab: true
            onClicked: root.close()
        }
    }
}
