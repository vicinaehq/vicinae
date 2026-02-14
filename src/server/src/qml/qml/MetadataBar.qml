import QtQuick
import QtQuick.Layouts

/// Displays key-value metadata rows with labels left-aligned and values right-aligned.
/// Each entry in `model` is an object with: label (string), value (string), icon (string, optional).
/// Scrolls vertically when content exceeds available height.
Item {
    id: root
    property var model: []
    implicitHeight: column.implicitHeight + 20  // 2 * margins

    Flickable {
        anchors.fill: parent
        anchors.margins: 10
        contentHeight: column.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: column
            width: parent.width
            spacing: 10

            Repeater {
                model: root.model

                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: modelData.label || ""
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }

                    Item { Layout.fillWidth: true }

                    Image {
                        visible: (modelData.icon || "") !== ""
                        source: modelData.icon || ""
                        sourceSize.width: 14
                        sourceSize.height: 14
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                    }

                    Text {
                        text: modelData.value || ""
                        color: Theme.foreground
                        font.pointSize: Theme.smallerFontSize
                        elide: Text.ElideMiddle
                        Layout.maximumWidth: root.width * 0.65
                    }
                }
            }
        }
    }
}
