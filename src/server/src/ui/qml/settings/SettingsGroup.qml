pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

// Grouped "inset card" container for settings rows.
SourceBlendRect {
    id: root
    default property alias content: inner.data

    Layout.fillWidth: true
    implicitHeight: inner.implicitHeight

    radius: 12
    backgroundColor: Theme.background
    color: Theme.secondaryBackground

    ColumnLayout {
        id: inner
        width: parent.width
        spacing: 0
    }
}
