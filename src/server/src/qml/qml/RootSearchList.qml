import QtQuick
import QtQuick.Controls

GenericListView {
    id: searchListView
    model: searchModel
    listModel: searchModel
    autoWireModel: true

    delegate: Loader {
        id: delegateLoader
        width: ListView.view.width

        required property int index
        required property bool isSection
        required property bool isSelectable
        required property string sectionName
        required property string itemType
        required property string title
        required property string subtitle
        required property string iconSource
        required property string alias
        required property bool isActive
        required property string accessoryText
        required property string accessoryColor
        required property bool isCalculator
        required property string calcQuestion
        required property string calcQuestionUnit
        required property string calcAnswer
        required property string calcAnswerUnit
        required property bool isFile

        sourceComponent: {
            if (isSection) return sectionComponent
            if (isCalculator) return calculatorComponent
            return itemComponent
        }

        Component {
            id: sectionComponent
            SectionHeader {
                width: delegateLoader.width
                text: delegateLoader.sectionName
            }
        }

        Component {
            id: calculatorComponent
            CalculatorResultDelegate {
                width: delegateLoader.width
                calcQuestion: delegateLoader.calcQuestion
                calcQuestionUnit: delegateLoader.calcQuestionUnit
                calcAnswer: delegateLoader.calcAnswer
                calcAnswerUnit: delegateLoader.calcAnswerUnit
                selected: searchListView.currentIndex === delegateLoader.index
                onClicked: searchListView.currentIndex = delegateLoader.index
                onDoubleClicked: searchListView.itemActivated(delegateLoader.index)
            }
        }

        Component {
            id: itemComponent
            ListItemDelegate {
                width: delegateLoader.width
                itemTitle: delegateLoader.title
                itemSubtitle: delegateLoader.subtitle
                itemIconSource: delegateLoader.iconSource
                itemAlias: delegateLoader.alias
                itemIsActive: delegateLoader.isActive
                itemAccessory: delegateLoader.accessoryText
                itemAccessoryColor: delegateLoader.accessoryColor
                selected: searchListView.currentIndex === delegateLoader.index
                onClicked: searchListView.currentIndex = delegateLoader.index
                onDoubleClicked: searchListView.itemActivated(delegateLoader.index)
            }
        }
    }
}
