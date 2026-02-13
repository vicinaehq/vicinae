import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 400
    height: 600
    visible: true
    title: "Basic Window"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2c3e50"

            TextField {
                anchors.fill: parent
                anchors.margins: 8
                placeholderText: "Search or type here..."
                background: Rectangle {
                    radius: 4
                    color: "#ecf0f1"
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ecf0f1"

            Text {
                anchors.centerIn: parent
                text: "Content Area"
                font.pixelSize: 20
                color: "#7f8c8d"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2c3e50"

            Text {
                anchors.centerIn: parent
                text: "Footer"
                color: "white"
                font.pixelSize: 14
            }
        }
    }
}
