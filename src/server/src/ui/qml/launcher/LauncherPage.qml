pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls

Item {
    id: root
    required property url viewUrl
    required property var viewProperties
    property real headerInset: 0
    property real topInset: headerInset
    readonly property alias view: loader.item

    // A covered page keeps its geometry and scroll position.
    Binding {
        target: root
        property: "topInset"
        value: root.headerInset
        when: root.visible
        restoreMode: Binding.RestoreNone
    }

    Loader {
        id: loader
        y: root.topInset
        width: root.width
        height: Math.max(0, root.height - y)
    }

    Component.onCompleted: loader.setSource(viewUrl, viewProperties)

    function restoreFocus() {
        // qmllint disable missing-property
        if (loader.item && typeof loader.item.restoreFocus === "function")
            loader.item.restoreFocus();
        // qmllint enable missing-property
    }

    StackView.onActivated: Qt.callLater(restoreFocus)
}
