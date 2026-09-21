pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

LauncherView {
    id: root
    required property QuickAIViewHost host

    function restoreFocus() {
        composer.forceActiveFocus();
    }

    header: Item {
        implicitHeight: root.appearance.searchBarHeight
        height: implicitHeight

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ViciImage {
                Layout.leftMargin: 16
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                source: Img.icon(BuiltinIcon.ChevronLeft).withFillColor(Theme.textMuted)
                opacity: backHover.hovered ? 0.6 : 1.0

                HoverHandler {
                    id: backHover
                    cursorShape: Qt.PointingHandCursor
                }

                TapHandler {
                    onTapped: Launcher.goBack()
                }
            }

            Text {
                text: Launcher.navigationTitle
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.regularFontSize
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.rightMargin: 16
            }
        }
    }

    ScrollViewport {
        id: viewport
        anchors.fill: parent
        flickable: flickable
        topPadding: 12
        bottomPadding: 12

        function scrollToBottom() {
            viewport.scrollTo(viewport.maximumY);
        }

        Flickable {
            id: flickable
            anchors.fill: parent
            contentWidth: width
            contentHeight: mainCol.height + 12
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ViciWheelHandler {
                target: flickable
            }

            ScrollBar.vertical: ViciScrollBar {}

            Column {
                id: mainCol
                x: 16
                width: flickable.width - 32
                spacing: 16

                Repeater {
                    model: root.host.exchanges

                    delegate: Rectangle {
                        id: ex
                        required property var modelData
                        readonly property bool failed: ex.modelData.error !== ""
                        width: mainCol.width
                        height: cardCol.height + 24
                        radius: 8
                        color: ex.failed ? Config.withAlpha(Theme.danger, 0.06) : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.04)
                        border.color: ex.failed ? Config.withAlpha(Theme.danger, 0.35) : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.12)
                        border.width: 1

                        Column {
                            id: cardCol
                            x: 12
                            y: 12
                            width: parent.width - 24
                            spacing: 8

                            Text {
                                width: parent.width
                                text: ex.modelData.query
                                wrapMode: Text.Wrap
                                color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
                                font.family: Theme.fontFamily
                                font.pointSize: Theme.regularFontSize
                            }

                            MarkdownInline {
                                visible: !ex.failed
                                width: parent.width
                                height: visible ? implicitHeight : 0
                                markdown: ex.modelData.response
                                activeFocusOnTab: false
                            }

                            Text {
                                visible: ex.failed
                                width: parent.width
                                text: ex.modelData.error
                                wrapMode: Text.Wrap
                                color: Theme.danger
                                font.family: Theme.fontFamily
                                font.pointSize: Theme.regularFontSize
                            }
                        }
                    }
                }

                Rectangle {
                    visible: root.host.streaming
                    width: mainCol.width
                    height: streamCol.height + 24
                    radius: 8
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.04)
                    border.color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.12)
                    border.width: 1

                    Column {
                        id: streamCol
                        x: 12
                        y: 12
                        width: parent.width - 24
                        spacing: 8

                        Text {
                            width: parent.width
                            text: root.host.streamingQuery
                            wrapMode: Text.Wrap
                            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
                            font.family: Theme.fontFamily
                            font.pointSize: Theme.regularFontSize
                        }

                        Item {
                            width: parent.width
                            height: visible ? 24 : 0
                            visible: root.host.streaming && root.host.streamingContent.length === 0

                            PulsingDots {
                                active: parent.visible
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MarkdownInline {
                            width: parent.width
                            height: implicitHeight
                            markdown: root.host.streamingContent
                            visible: root.host.streamingContent.length > 0
                            enabled: false
                        }
                    }
                }

                Row {
                    visible: !root.host.streaming && root.host.modelLabel.length > 0
                    spacing: 6

                    ViciImage {
                        visible: root.host.modelIcon.valid
                        source: root.host.modelIcon
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: root.host.modelLabel
                        color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.35)
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.smallerFontSize
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }

    EmptyView {
        width: viewport.width
        height: viewport.height - viewport.bottomInset
        visible: root.host.exchanges.length === 0 && !root.host.streaming
        icon: Launcher.navigationIcon
        title: qsTr("Ask anything")
        description: qsTr("Answers use the model selected in the composer.")
    }

    footer: Item {
        implicitHeight: composer.height + 20
        height: implicitHeight

        ChatComposer {
            id: composer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            placeholder: root.host.exchanges.length === 0 && !root.host.streaming ? qsTr("Ask anything...") : qsTr("Ask a follow-up...")
            busy: root.host.streaming
            modelItems: root.host.modelSelectorItems
            currentModel: root.host.modelSelectorCurrentItem
            dictationAvailable: root.host.dictationAvailable
            recording: root.host.recording
            transcribing: root.host.transcribing
            recordingTime: root.host.recordingTime
            dictationMessage: root.host.dictationMessage
            onSubmitted: text => root.host.send(text)
            onCancelled: root.host.cancel()
            onModelActivated: item => root.host.selectModel(item.id)
            onDictationToggled: root.host.toggleDictation()
            onDictationCancelled: root.host.cancelDictation()

            onHeightChanged: {
                if (flickable.contentY >= viewport.maximumY - composer.lineHeight)
                    Qt.callLater(viewport.scrollToBottom);
            }
        }
    }

    Component.onCompleted: composer.forceActiveFocus()

    Connections {
        target: root.host
        function onDictated(text) {
            composer.insertText(text);
        }
        function onStreamingChanged() {
            Qt.callLater(viewport.scrollToBottom);
        }
        function onStreamingContentChanged() {
            viewport.scrollToBottom();
        }
        function onExchangesChanged() {
            Qt.callLater(viewport.scrollToBottom);
        }
    }
}
