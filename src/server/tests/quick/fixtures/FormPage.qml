import QtQuick
import QtQuick.Layouts
import Vicinae

FormView {
    property alias fields: fields

    Repeater {
        id: fields
        model: 20

        TextInput {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            activeFocusOnTab: true
            text: "Field"
        }
    }
}
