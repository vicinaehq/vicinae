import QtQuick

Window {
    property real statusBarOverlap: 48
    property real statusBarTop: height - statusBarOverlap
    property QtObject appearance: QtObject {
        property real contentBottomInset: 24
    }
}
