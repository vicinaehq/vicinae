pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Documents
import Vicinae.Scrolling as Scrolling

Control {
    id: root
    default property alias contentData: contents.data
    property DocumentStyle style: root.DocumentScope.style
    property alias typingTarget: controller.typingTarget
    readonly property alias flickable: viewport
    readonly property alias contentHeight: viewport.contentHeight
    readonly property DocumentController document: DocumentController {
        id: controller
        container: contents
        flickable: viewport
        onLinkActivated: link => root.linkActivated(link)
    }
    signal linkActivated(string link)

    padding: 12
    spacing: 12
    implicitHeight: contents.implicitHeight + topPadding + bottomPadding

    contentItem: Flickable {
        id: viewport
        DocumentScope.document: controller
        DocumentScope.style: root.style
        contentWidth: width
        contentHeight: contents.height
        clip: true
        activeFocusOnTab: true
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Scrolling.WheelHandler {
            target: viewport.contentHeight > viewport.height ? viewport : null
            blockTargetWheel: false
            keyNavigationEnabled: true
        }
        ScrollBar.vertical: DocumentScrollBar {}

        Column {
            id: contents
            width: viewport.width
            spacing: root.spacing
        }
    }

    ContextMenu.menu: DocumentSelectionMenu {
        controller: root.document
        onAboutToShow: viewport.forceActiveFocus()
    }
}
