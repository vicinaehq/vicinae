pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root
    property string name: ""
    property string summary: ""
    property string status: "queued"
    property var statusText: null
    property var durationMs: null
    property bool expanded: false
    property string arguments: ""
    property string output: ""
    property bool hasOutput: false
    property Item typingTarget: null
    property real maximumDetailsHeight: 240
    signal toggled

    implicitHeight: header.height + (expanded ? details.height + 4 : 0)

    ToolInvocationHeader {
        id: header
        width: parent.width
        title: root.summary.length > 0 ? root.summary : root.name
        monospace: root.summary.length > 0
        compactStatus: true
        status: root.status
        resultStatusText: root.statusText
        durationMs: root.durationMs
        expanded: root.expanded
        onToggled: root.toggled()
    }

    Loader {
        id: details
        y: header.height + 4
        width: parent.width
        active: root.expanded
        sourceComponent: Rectangle {
            id: panel
            property bool showArguments: false
            implicitHeight: Math.min(root.maximumDetailsHeight, scroll.contentHeight + 2)
            radius: 6
            color: Config.withAlpha(Theme.foreground, 0.025)
            border.color: Config.withAlpha(Theme.foreground, 0.1)
            DocumentScope.document: selection

            ContextMenu.menu: DocumentSelectionMenu {
                id: detailsMenu
                controller: selection
                onAboutToShow: scroll.forceActiveFocus()
                Instantiator {
                    model: root.summary.length > 0 ? 1 : 0
                    delegate: MenuItem {
                        text: qsTr("Show Arguments")
                        checkable: true
                        checked: panel.showArguments
                        onTriggered: panel.showArguments = checked
                    }
                    onObjectAdded: (index, object) => detailsMenu.addItem(object)
                    onObjectRemoved: (index, object) => detailsMenu.removeItem(object)
                }
            }

            DocumentController {
                id: selection
                container: scroll.contentItem
                flickable: scroll
                typingTarget: root.typingTarget
            }

            Flickable {
                id: scroll
                anchors.fill: parent
                anchors.margins: 1
                contentWidth: width
                contentHeight: body.height + 20
                interactive: contentHeight > height
                boundsBehavior: Flickable.StopAtBounds
                flickableDirection: Flickable.VerticalFlick
                clip: true

                ViciWheelHandler {
                    target: scroll.interactive ? scroll : null
                    blockTargetWheel: false
                    keyNavigationEnabled: true
                }
                ScrollBar.vertical: ViciScrollBar {
                    bottomPadding: 0
                }

                Column {
                    id: body
                    x: 10
                    y: 10
                    width: scroll.width - 24
                    spacing: 6

                    DocumentText {
                        width: parent.width
                        text: panel.showArguments || root.summary.length === 0 ? root.arguments : root.summary
                        font.family: Theme.monoFontFamily
                        font.pointSize: Theme.smallerFontSize
                        color: Theme.textMuted
                    }
                    Rectangle {
                        width: parent.width
                        height: 1
                        visible: root.hasOutput
                        color: Config.withAlpha(Theme.foreground, 0.1)
                    }
                    DocumentText {
                        width: parent.width
                        visible: root.hasOutput && root.output.length > 0
                        text: root.output.replace(/\r?\n$/, "")
                        font.family: Theme.monoFontFamily
                        font.pointSize: Theme.smallerFontSize
                    }
                    Text {
                        visible: root.hasOutput && root.output.length === 0
                        text: qsTr("No output")
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.smallerFontSize
                        color: Theme.textMuted
                    }
                }
            }
        }
    }
}
