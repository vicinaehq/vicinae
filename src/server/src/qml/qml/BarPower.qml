import QtQuick

BarPill {
    id: root

    visible: bar.powerActions.length > 0
    tooltip: qsTr("Power")
    horizontalPadding: 6
    highlighted: popover.visible
    onClicked: popover.open()

    ViciImage {
        width: 18
        height: 18
        source: Img.builtin("power").withFillColor(Theme.foreground)
        sourceSize: Qt.size(18, 18)
    }

    BarPopover {
        id: popover
        anchorItem: root
        minimumWidth: 170

        contentItem: Column {
            width: popover.minimumWidth
            spacing: 0

            Repeater {
                model: bar.powerActions

                BarMenuRow {
                    required property var modelData
                    width: parent.width
                    label: modelData.label
                    showIconSlot: true
                    iconSource: Img.builtin(modelData.icon).withFillColor(Theme.foreground)
                    onClicked: {
                        popover.close();
                        bar.powerAction(modelData.id);
                    }
                }
            }
        }
    }
}
