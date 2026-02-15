.pragma library

function isTextEditingShortcut(key, modifiers) {
    var ctrl = Qt.ControlModifier
    var ctrlShift = Qt.ControlModifier | Qt.ShiftModifier

    if (modifiers === ctrl) {
        switch (key) {
        case Qt.Key_A: case Qt.Key_C: case Qt.Key_V: case Qt.Key_X:
        case Qt.Key_Z: case Qt.Key_Y:
        case Qt.Key_Left: case Qt.Key_Right: case Qt.Key_Home: case Qt.Key_End:
        case Qt.Key_Backspace: case Qt.Key_Delete:
            return true
        }
    } else if (modifiers === ctrlShift) {
        switch (key) {
        case Qt.Key_Z:
        case Qt.Key_Left: case Qt.Key_Right: case Qt.Key_Home: case Qt.Key_End:
            return true
        }
    }
    return false
}
