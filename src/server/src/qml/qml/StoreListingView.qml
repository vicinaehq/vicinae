import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    function moveUp() {
        listView.moveUp();
    }
    function moveDown() {
        listView.moveDown();
    }
    function moveSectionUp() {
        listView.moveSectionUp();
    }
    function moveSectionDown() {
        listView.moveSectionDown();
    }

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
            required property var itemAccessory
            required property string downloadCount
            required property string authorAvatar
            required property bool isInstalled
            required property int compatTier

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
                    opacity: delegateLoader.compatTier === 2 ? 0.5 : 1.0
                    selected: listView.currentIndex === delegateLoader.index
                    onClicked: listView.currentIndex = delegateLoader.index
                    onActivated: listView.itemActivated(delegateLoader.index)

                    readonly property var compatColors: [Theme.toastSuccess, Theme.toastWarning, Theme.toastDanger, Theme.textMuted]

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 15

                        Rectangle {
                            Layout.preferredWidth: 8
                            Layout.preferredHeight: 8
                            Layout.alignment: Qt.AlignVCenter
                            radius: 4
                            color: itemDelegate.compatColors[delegateLoader.compatTier] ?? Theme.textMuted
                        }

                        ViciImage {
                            Layout.preferredWidth: 30
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignVCenter
                            source: delegateLoader.iconSource
                        }

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

                        ViciImage {
                            visible: delegateLoader.isInstalled
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            Layout.alignment: Qt.AlignVCenter
                            source: Img.builtin("check-circle").withFillColor(Theme.toastSuccess)
                        }

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

                        ViciImage {
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            Layout.alignment: Qt.AlignVCenter
                            source: delegateLoader.authorAvatar
                        }
                    }
                }
            }
        }
    }
}
