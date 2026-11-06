pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property SnippetFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            ColumnLayout {
                id: leftColumn
                Layout.fillWidth: true
                Layout.preferredWidth: 3
                Layout.alignment: Qt.AlignTop
                spacing: 12

                FormField {
                    id: titleField
                    stacked: true
                    label: qsTr("Title")
                    error: root.host.nameError

                    FormTextInput {
                        text: root.host.name
                        placeholder: qsTr("Euro symbol")
                        hasError: titleField.error !== ""
                        onTextEdited: root.host.name = text
                    }
                }

                FormField {
                    id: contentField
                    stacked: true
                    label: qsTr("Content")
                    error: root.host.contentError
                    info: qsTr("You can use {dynamic placeholders} to make the content dynamic: <a href=\"https://docs.vicinae.com/snippets\">learn more</a>.")

                    FormTextArea {
                        // fills what the form has left, the text scrolls inside when it needs more
                        readonly property int rows: Math.max(5, Math.floor((formView.usableHeight - formView.padding * 2 - titleField.height - leftColumn.spacing - contentField.stackedChromeHeight - _verticalPadding * 2) / _lineHeight))
                        minRows: rows
                        maxRows: rows
                        text: root.host.content
                        placeholder: "€"
                        completions: root.host.contentCompletions
                        highlighter: PlaceholderHighlight {}
                        hasError: contentField.error !== ""
                        onTextEdited: root.host.content = text
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredWidth: 2
                Layout.alignment: Qt.AlignTop
                spacing: 12

                FormField {
                    id: keywordField
                    stacked: true
                    label: qsTr("Keyword")
                    error: root.host.keywordError
                    info: root.host.serverRunning ? qsTr("Typing this keyword anywhere will result in it being replaced by the content of the snippet.") : qsTr("The snippet server is not running. Keyword expansion is unavailable. <a href=\"https://docs.vicinae.com/snippets\">Learn more</a>.")

                    FormTextInput {
                        text: root.host.keyword
                        placeholder: ":!euro"
                        hasError: keywordField.error !== ""
                        readOnly: !root.host.serverRunning
                        onTextEdited: root.host.keyword = text
                    }
                }

                FormField {
                    stacked: true
                    info: qsTr("If a keyword is typed, it will only be expanded after space or punctuation.")
                    visible: root.host.serverRunning && root.host.keyword !== ""

                    FormCheckbox {
                        label: qsTr("Expand as word")
                        checked: root.host.expandAsWord
                        onToggled: root.host.expandAsWord = checked
                    }
                }

                FormField {
                    id: voicePhraseField
                    stacked: true
                    label: qsTr("Voice phrase")
                    error: root.host.voicePhraseError
                    info: qsTr("Dictating exactly this phrase inserts the snippet instead.")

                    FormTextInput {
                        leadingIcon: Img.icon(BuiltinIcon.Microphone).withFillColor(Theme.textMuted)
                        text: root.host.voicePhrase
                        placeholder: qsTr("euro symbol")
                        hasError: voicePhraseField.error !== ""
                        onTextEdited: root.host.voicePhrase = text
                    }
                }

                FormField {
                    stacked: true
                    label: qsTr("Applications")
                    info: qsTr("Restrict the keyword and voice phrase to specific applications. By default, they work everywhere.")
                    visible: (root.host.serverRunning && root.host.keyword !== "") || root.host.voicePhrase !== ""

                    FormAppSelector {
                        model: root.host.apps
                        appsModel: root.host.appSelectorModel.model
                        placeholder: qsTr("+ Add app…")
                        emptyText: ""
                        onChanged: apps => root.host.apps = apps
                    }
                }
            }
        }
    }
}
