import QtQuick

Rectangle {
    id: chip
    
    property string chipId
    property string text: ""
    property var imageSource

    signal removeRequested
    signal focusOnPreviousChipRequested
    signal focusOnNextChipRequested

    width: row.width + 16
    height: 28
    radius: 4
    color: Qt.rgba(Theme.listItemSelectionBg.r, Theme.listItemSelectionBg.g, Theme.listItemSelectionBg.b, Config.windowOpacity)
    border.color: activeFocus ? Theme.inputBorderFocus : "transparent"
    border.width: 1

    activeFocusOnTab: true

    Keys.onPressed: event => {
                        if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            removeRequested()
                            event.accepted = true
                        } else if (event.key === Qt.Key_Left) {
                            focusOnPreviousChipRequested()
                            event.accepted = true
                        } else if (event.key === Qt.Key_Right) {
                            focusOnNextChipRequested()
                            event.accepted = true
                        }
                    }

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 6

        Row {
          Loader {
            id: imageLoader
            active: chip.imageSource !== undefined && chip.imageSource !== ""
            anchors.verticalCenter: parent.verticalCenter

            sourceComponent: Component {
              ViciImage {
                anchors.verticalCenter: parent.verticalCenter
                width: 20
                height: 20
                source: chip.imageSource
              }
            }
          }
          Text {
              text: chip.text
              anchors.verticalCenter: parent.verticalCenter
              color: Theme.foreground
          }
        }
        ViciImage {
            source: Img.builtin("xmark")
            height: 10
            width: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: chip.removeRequested()
    }
}

