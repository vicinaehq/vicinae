import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Rectangle {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null

    readonly property string language: blockData.language ?? ""
    readonly property string code: blockData.code ?? ""
    readonly property string highlightedHtml: blockData.highlightedHtml ?? ""

    readonly property string _langIcon: {
        var lang = language.toLowerCase()
        var map = {
            "python": "python", "py": "python",
            "javascript": "javascript", "js": "javascript",
            "typescript": "typescript", "ts": "typescript",
            "rust": "rust", "rs": "rust",
            "go": "go", "golang": "go",
            "java": "java",
            "kotlin": "kotlin", "kt": "kotlin",
            "swift": "swift",
            "ruby": "ruby", "rb": "ruby",
            "php": "php",
            "csharp": "csharp", "cs": "csharp", "c#": "csharp",
            "dart": "dart",
            "lua": "lua",
            "perl": "perl", "pl": "perl",
            "r": "r",
            "scala": "scala",
            "haskell": "haskell", "hs": "haskell",
            "elixir": "elixir", "ex": "elixir",
            "bash": "bash", "sh": "bash", "shell": "bash", "zsh": "bash",
            "json": "json",
            "yaml": "yaml", "yml": "yaml",
            "xml": "xml",
            "html": "html5",
            "css": "css3",
            "graphql": "graphql", "gql": "graphql",
            "markdown": "markdown", "md": "markdown",
            "docker": "docker", "dockerfile": "docker",
            "react": "react", "jsx": "react", "tsx": "react",
            "svelte": "svelte",
            "angular": "angular",
        }
        return map[lang] ?? ""
    }

    width: parent?.width ?? 0
    implicitHeight: col.implicitHeight
    radius: 6
    color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                   Theme.secondaryBackground.b, 0.6)
    border.width: 1
    border.color: Theme.divider

    ColumnLayout {
        id: col
        anchors.fill: parent
        spacing: 0

        // Header bar with language icon + label + copy button
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: headerRow.implicitHeight + 16
            color: "transparent"
            visible: true

            RowLayout {
                id: headerRow
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 8
                anchors.bottomMargin: 8
                spacing: 5

                ViciImage {
                    Layout.preferredWidth: 14
                    Layout.preferredHeight: 14
                    visible: root._langIcon.length > 0
                    source: root._langIcon.length > 0
                            ? Img.builtin(root._langIcon).withFillColor(Theme.textMuted)
                            : Img.builtin("code")
                }

                Text {
                    text: root.language
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize * 0.9
                    visible: root.language.length > 0
                }

                Item { Layout.fillWidth: true }

                Text {
                    id: copyBtn
                    text: copyTimer.running ? "Copied!" : "Copy"
                    color: copyMouse.containsMouse ? Theme.foreground : Theme.textMuted
                    font.pointSize: Theme.smallerFontSize * 0.9

                    MouseArea {
                        id: copyMouse
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            if (root.mdModel)
                                root.mdModel.copyCodeBlock(root.blockIndex)
                            copyTimer.restart()
                        }
                    }

                    Timer {
                        id: copyTimer
                        interval: 2000
                    }
                }
            }

            // Divider below header
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: Theme.divider
            }
        }

        Flickable {
            id: codeFlickable
            Layout.fillWidth: true
            Layout.margins: 10
            implicitHeight: codeEdit.contentHeight
            contentWidth: Math.max(width, codeEdit.contentWidth)
            contentHeight: codeEdit.contentHeight
            clip: true
            interactive: false
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.horizontal: ViciScrollBar {
                policy: codeFlickable.contentWidth > codeFlickable.width
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            TextEdit {
                id: codeEdit
                width: Math.max(codeFlickable.width, contentWidth)
                readOnly: true
                selectionColor: Theme.textSelectionBg
                selectedTextColor: Theme.textSelectionFg
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.NoWrap
                color: Theme.foreground
                font.family: "monospace"
                font.pointSize: Theme.regularFontSize * 0.95
                text: root.highlightedHtml
            }
        }
    }

    onSelectionControllerChanged: if (selectionController) selectionController.registerSelectable(codeEdit, blockIndex * 10000, true)
    Component.onDestruction: if (selectionController) selectionController.unregisterSelectable(codeEdit)
}
