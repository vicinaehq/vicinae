pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Flickable {
    id: root
    required property DocumentModel documentModel
    readonly property alias document: controller
    property alias typingTarget: controller.typingTarget
    property alias topInset: controller.topInset
    property alias bottomInset: controller.bottomInset
    property alias delegate: geometry.delegate
    property alias spacing: geometry.spacing
    property alias cacheBuffer: geometry.cacheBuffer
    readonly property alias count: geometry.count
    readonly property alias layoutChanging: geometry.updating
    readonly property real minimumY: originY - topMargin
    readonly property real maximumY: Math.max(minimumY, originY + contentHeight - height + bottomMargin)
    property Component footer
    property bool followEnd: false
    property var layoutKey: [Theme.fontFamily, Theme.monoFontFamily, Theme.regularFontSize, Theme.smallerFontSize]
    signal layoutUpdated

    DocumentScope.document: controller
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    contentWidth: width
    contentHeight: geometry.contentHeight + footerLoader.height
    activeFocusOnTab: true

    function scrollToBeginning() {
        contentY = minimumY;
        geometry.forceLayout();
    }
    function scrollToEnd() {
        geometry.positionAtEnd();
    }
    function revealPosition(row: int, part: int, position: int, length: int) {
        geometry.positionAt(row, part, position, length);
    }

    onLayoutKeyChanged: geometry.invalidate()

    DocumentLayout {
        id: geometry
        model: root.documentModel
        viewport: root
        followEnd: root.followEnd
        cacheBuffer: root.height
        onLayoutUpdated: root.layoutUpdated()
    }
    Loader {
        id: footerLoader
        y: geometry.contentHeight
        sourceComponent: root.footer
    }
    DocumentController {
        id: controller
        model: root.documentModel
        container: root.contentItem
        flickable: root
    }
}
