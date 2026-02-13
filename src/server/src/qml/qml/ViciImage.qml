import QtQuick

Item {
    id: root
    property var source

    Image {
        anchors.fill: parent
        source: {
            if (!root.source || !root.source.valid) return ""
            // Access Theme.foreground to create a binding dependency on theme changes.
            // toSource() resolves current theme colors, so the URL changes when theme changes.
            if (root.source.isThemeSensitive) { var _ = Theme.foreground }
            return root.source.toSource()
        }
        asynchronous: true
        cache: true
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
}
