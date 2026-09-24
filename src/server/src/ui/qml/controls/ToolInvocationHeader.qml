pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Rectangle {
    id: root
    property string title: ""
    property string subtitle: ""
    property var iconSource
    property string status: "queued"
    property var durationMs: null
    property bool expanded: false
    property bool compactStatus: false
    property var resultStatusText: null
    signal toggled

    readonly property bool running: status === "running"
    property string statusText: {
        if (resultStatusText !== null && resultStatusText !== undefined)
            return resultStatusText;
        switch (status) {
        case "running":
            return qsTr("Running…");
        case "succeeded":
        case "completed":
            return qsTr("Completed");
        case "failed":
            return qsTr("Failed");
        case "cancelled":
            return qsTr("Cancelled");
        default:
            return qsTr("Queued");
        }
    }
    readonly property string durationText: {
        if (durationMs === null || durationMs === undefined)
            return "";
        if (durationMs < 1000)
            return "%1 ms".arg(durationMs.toLocaleString(Qt.locale(), 'f', 0));
        return "%1 s".arg((durationMs / 1000).toLocaleString(Qt.locale(), 'f', 1));
    }
    readonly property string statusLabel: durationText.length > 0 ? qsTr("%1 · %2").arg(statusText).arg(durationText) : statusText

    implicitHeight: subtitle.length > 0 ? 50 : 32
    radius: 6
    color: mouse.containsMouse || activeFocus ? Theme.listItemHoverBg : "transparent"
    activeFocusOnTab: true
    Accessible.role: Accessible.Button
    Accessible.name: root.subtitle.length > 0 ? qsTr("%1 — %2 — %3").arg(root.title).arg(root.subtitle).arg(root.statusLabel) : qsTr("%1 — %2").arg(root.title).arg(root.statusLabel)
    Accessible.description: root.expanded ? qsTr("Collapse tool details") : qsTr("Expand tool details")
    Accessible.onPressAction: root.toggled()
    Keys.onReturnPressed: root.toggled()
    Keys.onSpacePressed: root.toggled()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 8
        spacing: 8
        ViciImage {
            Layout.preferredWidth: 12
            Layout.preferredHeight: 12
            source: Img.icon(root.expanded ? BuiltinIcon.ChevronDown : BuiltinIcon.ChevronRight).withFillColor(Theme.textMuted)
        }
        ViciImage {
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            source: root.iconSource || Img.icon(BuiltinIcon.WrenchScrewdriver).withFillColor(Theme.textMuted)
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text {
                Layout.fillWidth: true
                text: root.title.replace(/\s+/g, " ")
                textFormat: Text.PlainText
                elide: Text.ElideRight
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                color: Theme.foreground
            }
            Text {
                Layout.fillWidth: true
                visible: root.subtitle.length > 0
                text: root.subtitle.replace(/\s+/g, " ")
                textFormat: Text.PlainText
                elide: Text.ElideRight
                font.family: Theme.monoFontFamily
                font.pointSize: Theme.smallerFontSize
                color: Theme.textMuted
            }
        }
        ViciSpinner {
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            visible: root.running
        }
        Text {
            Layout.maximumWidth: root.width * 0.45
            text: root.compactStatus && (root.status === "succeeded" || root.status === "completed") && root.durationText.length > 0 ? root.durationText : root.statusLabel
            textFormat: Text.PlainText
            elide: Text.ElideRight
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            color: root.status === "failed" ? Theme.danger : Theme.textMuted
        }
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled()
    }
}
