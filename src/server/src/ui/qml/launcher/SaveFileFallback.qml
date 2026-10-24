import QtQuick
import QtQuick.Dialogs
import Vicinae

FileDialog {
    id: root

    title: qsTr("Save as")
    fileMode: FileDialog.SaveFile
    onAccepted: FileChooser.acceptFallbackSave(selectedFile)
    onRejected: FileChooser.rejectFallbackSave()

    Connections {
        target: FileChooser

        function onFallbackSaveRequested(suggestedFile) {
            root.selectedFile = suggestedFile;
            root.open();
        }

        function onDialogClosed() {
            if (root.visible)
                root.close();
        }
    }
}
