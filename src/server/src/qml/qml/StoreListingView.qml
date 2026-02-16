import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    function moveUp() { listView.moveUp() }
    function moveDown() { listView.moveDown() }

    GenericListView {
        id: listView
        anchors.fill: parent
        listModel: root.host.listModel
        model: root.host.listModel
        autoWireModel: true

        delegate: Loader {
            id: delegateLoader
            width: ListView.view.width

            required property int index
            required property bool isSection
            required property bool isSelectable
            required property string sectionName
            required property string title
            required property string subtitle
            required property string iconSource
            required property string itemAccessory
            required property string downloadCount
            required property string authorAvatar
            required property bool isInstalled

            sourceComponent: isSection ? sectionComponent : itemComponent

            Component {
                id: sectionComponent
                SectionHeader {
                    width: delegateLoader.width
                    text: delegateLoader.sectionName
                }
            }

            Component {
                id: itemComponent
                SelectableDelegate {
                    id: itemDelegate
                    width: delegateLoader.width
                    height: 50
                    selected: listView.currentIndex === delegateLoader.index
                    onClicked: listView.currentIndex = delegateLoader.index
                    onDoubleClicked: listView.itemActivated(delegateLoader.index)

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 15

                        // Extension icon
                        Image {
                            Layout.preferredWidth: 30
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignVCenter
                            source: delegateLoader.iconSource
                            sourceSize.width: 30
                            sourceSize.height: 30
                            asynchronous: true
                            cache: true
                        }

                        // Title + description
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Text {
                                text: delegateLoader.title
                                color: itemDelegate.selected ? Theme.listItemSelectionFg : Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                elide: Text.ElideRight
                                maximumLineCount: 1
                                Layout.fillWidth: true
                            }

                            Text {
                                text: delegateLoader.subtitle
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                                elide: Text.ElideRight
                                maximumLineCount: 1
                                Layout.fillWidth: true
                            }
                        }

                        // Installed badge
                        ViciImage {
                            visible: delegateLoader.isInstalled
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            Layout.alignment: Qt.AlignVCenter
                            source: Img.builtin("check-circle").withFillColor(Theme.toastSuccess)
                        }

                        // Download count
                        RowLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignVCenter

                            ViciImage {
                                Layout.preferredWidth: 14
                                Layout.preferredHeight: 14
                                source: Img.builtin("arrow-down-circle").withFillColor(Theme.textMuted)
                            }

                            Text {
                                text: delegateLoader.downloadCount
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }

                        // Author avatar
                        Image {
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            Layout.alignment: Qt.AlignVCenter
                            source: delegateLoader.authorAvatar
                            sourceSize.width: 20
                            sourceSize.height: 20
                            asynchronous: true
                            cache: true
                        }
                    }
                }
            }
        }
    }
}
