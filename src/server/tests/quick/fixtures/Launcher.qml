pragma Singleton
import QtQuick

QtObject {
    readonly property bool compacted: false
    readonly property bool hasOverlay: false
    readonly property bool filePicking: false
    readonly property QtObject alertModel: QtObject {
        readonly property bool visible: false
    }
}
