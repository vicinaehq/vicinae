pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ChatComposer {
    id: root
    required property ChatSession session
    signal messageSent

    text: session.draft
    onTextChanged: session.draft = text
    busy: session.streaming
    attachments: session.attachments
    submissionEnabled: session.canSend
    message: session.attachmentMessage
    modelItems: session.modelSelectorItems
    currentModel: session.modelSelectorCurrentItem
    dictationAvailable: session.dictationAvailable
    recording: session.recording
    transcribing: session.transcribing
    recordingTime: session.recordingTime
    dictationMessage: session.dictationMessage
    onSubmitted: text => {
        if (session.send(text)) {
            root.text = "";
            messageSent();
        }
    }
    onCancelled: session.cancel()
    onModelActivated: item => session.selectModel(item.id)
    onDictationToggled: session.toggleDictation()
    onDictationCancelled: session.cancelDictation()

    Connections {
        target: root.session
        function onDictated(text) {
            root.insertText(text);
        }
    }
}
