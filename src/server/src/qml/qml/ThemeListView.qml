import QtQuick
import QtQuick.Controls

GenericListView {
    id: themeListView
    property var cmdModel: null
    model: cmdModel
    listModel: cmdModel
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
        required property color paletteColor0
        required property color paletteColor1
        required property color paletteColor2
        required property color paletteColor3
        required property color paletteColor4
        required property color paletteColor5
        required property color paletteColor6
        required property color paletteColor7

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
            ThemeItemDelegate {
                width: delegateLoader.width
                itemTitle: delegateLoader.title
                itemSubtitle: delegateLoader.subtitle
                itemIconSource: delegateLoader.iconSource
                paletteColor0: delegateLoader.paletteColor0
                paletteColor1: delegateLoader.paletteColor1
                paletteColor2: delegateLoader.paletteColor2
                paletteColor3: delegateLoader.paletteColor3
                paletteColor4: delegateLoader.paletteColor4
                paletteColor5: delegateLoader.paletteColor5
                paletteColor6: delegateLoader.paletteColor6
                paletteColor7: delegateLoader.paletteColor7
                selected: themeListView.currentIndex === delegateLoader.index
                onClicked: themeListView.currentIndex = delegateLoader.index
                onDoubleClicked: themeListView.itemActivated(delegateLoader.index)
            }
        }
    }
}
