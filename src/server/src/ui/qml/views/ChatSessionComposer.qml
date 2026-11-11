pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ChatComposer {
    id: root
    required property ChatSession session
    signal messageSent

    text: session.draft
    onTextChanged: session.draft = text
    busy: session.streaming || session.searching
    attachments: session.attachments
    submissionEnabled: session.canSend
    message: session.attachmentMessage
    modelItems: session.modelSelectorItems
    currentModel: session.modelSelectorCurrentItem
    toolApproval: session.toolApproval
    onSubmitted: text => {
        if (session.send(text)) {
            root.text = "";
            messageSent();
        }
    }
    onCancelled: session.cancel()
    onModelActivated: item => session.selectModel(item.id)
    onToolApproved: always => session.approveTool(always)
    onToolDeclined: session.declineTool()
}
