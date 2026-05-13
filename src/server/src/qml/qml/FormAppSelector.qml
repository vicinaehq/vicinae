import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    Layout.fillWidth: true
    spacing: 6

    property var model: []
    property var sections: []

    signal changed(var apps)

    function _appInfo(wmClass) {
        for (let s = 0; s < sections.length; s++) {
            const items = sections[s].items;
            for (let i = 0; i < items.length; i++) {
                if (items[i].id === wmClass)
                    return items[i];
            }
        }
        return {
            displayName: wmClass,
            iconSource: ""
        };
    }

    function add(wmClass) {
        if (model.indexOf(wmClass) >= 0)
            return;
        let copy = model.slice();
        copy.push(wmClass);
        model = copy;
        changed(model);
    }

    function remove(wmClass) {
        let copy = [];
        for (let i = 0; i < model.length; i++) {
            if (model[i] !== wmClass)
                copy.push(model[i]);
        }
        model = copy;
        changed(model);
    }

    Text {
        visible: root.model.length === 0
        text: "All applications"
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        font.italic: true
    }

    Repeater {
        model: root.model

        Rectangle {
            required property int index
            required property var modelData

            readonly property var info: root._appInfo(modelData)

            Layout.fillWidth: true
            implicitHeight: 32
            radius: 6
            color: "transparent"
            border.color: Theme.inputBorder
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 6
                spacing: 6

                ViciImage {
                    source: info.iconSource ?? ""
                    visible: source !== ""
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                }

                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    text: info.displayName ?? modelData
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    elide: Text.ElideRight
                }

                ViciButton {
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignVCenter
                    radius: 4
                    icon: "xmark"
                    iconSize: 10
                    variant: "ghost"
                    onClicked: root.remove(modelData)
                }
            }
        }
    }

    SearchableDropdown {
        placeholder: "+ Restrict to app…"
        items: root.sections
        currentItem: null
        onActivated: item => root.add(item.id)
    }
}
