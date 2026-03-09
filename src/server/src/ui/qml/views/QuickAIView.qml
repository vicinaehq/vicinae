import QtQuick

Item {
    id: root
    required property var host

    function moveUp() {
        flickable.flick(0, 800);
    }
    function moveDown() {
        flickable.flick(0, -800);
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: mainCol.height + 24
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        function scrollToBottom() {
            if (contentHeight > height)
                contentY = contentHeight - height;
        }

        Column {
            id: mainCol
            y: 12
            x: 16
            width: flickable.width - 32
            spacing: 16

            // completed exchanges
            Repeater {
                model: host.exchanges

                delegate: Rectangle {
                    required property var modelData
                    width: mainCol.width
                    height: cardCol.height + 24
                    radius: 8
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.04)
                    border.color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.12)
                    border.width: 1

                    Column {
                        id: cardCol
                        x: 12
                        y: 12
                        width: parent.width - 24
                        spacing: 8

                        Text {
                            width: parent.width
                            text: modelData.query
                            wrapMode: Text.Wrap
                            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
                            font.family: Theme.fontFamily
                            font.pointSize: Theme.regularFontSize
                        }

                        MarkdownInline {
                            width: parent.width
                            height: implicitHeight
                            markdown: modelData.response
                            activeFocusOnTab: false
                        }
                    }
                }
            }

            // streaming card
            Rectangle {
                visible: host.streaming
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
                        text: host.streamingQuery
                        wrapMode: Text.Wrap
                        color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.regularFontSize
                    }

                    Item {
                        width: parent.width
                        height: visible ? 24 : 0
                        visible: host.streaming && host.streamingContent.length === 0

                        PulsingDots {
                            active: parent.visible
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MarkdownInline {
                        width: parent.width
                        height: implicitHeight
                        markdown: host.streamingContent
                        visible: host.streamingContent.length > 0
                        enabled: false
                    }
                }
            }

            // model label (only shown after generation completes)
            Row {
                visible: !host.streaming && host.modelLabel.length > 0
                spacing: 6

                ViciImage {
                    visible: host.modelIcon.valid
                    source: host.modelIcon
                    width: 20
                    height: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: host.modelLabel
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.35)
                    font.family: Theme.fontFamily
                    font.pointSize: Theme.smallFontSize
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    Connections {
        target: host
        function onStreamingChanged() {
            if (host.streaming)
                Qt.callLater(flickable.scrollToBottom);
        }
        function onStreamingContentChanged() {
            flickable.scrollToBottom();
        }
        function onExchangesChanged() {
            Qt.callLater(flickable.scrollToBottom);
        }
    }
}
