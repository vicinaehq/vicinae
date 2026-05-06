import QtQuick
import QtQuick.Controls

GenericListView {
    id: calcHistoryView
    property var cmdModel: null
    model: cmdModel
    listModel: cmdModel
    autoWireModel: true
    selectFirstOnReset: cmdModel ? cmdModel.selectFirstOnReset : true

    emptyTitle: cmdModel && cmdModel.emptyTitle || "No results"
    emptyDescription: (cmdModel && cmdModel.emptyDescription) || ""
    emptyIcon: {
        var _ = Theme.foreground;
        var icon = cmdModel ? (cmdModel.emptyIcon || "") : "";
        return icon !== "" ? icon : "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground;
    }

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
        required property bool isCalculator
        required property string calcQuestion
        required property string calcQuestionUnit
        required property string calcAnswer
        required property string calcAnswerUnit

        sourceComponent: {
            if (isSection)
                return sectionComponent;
            if (isCalculator)
                return calculatorComponent;
            return itemComponent;
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
                selected: calcHistoryView.currentIndex === delegateLoader.index
                onClicked: calcHistoryView.currentIndex = delegateLoader.index
                onActivated: calcHistoryView.itemActivated(delegateLoader.index)
            }
        }

        Component {
            id: itemComponent
            ListItemDelegate {
                width: delegateLoader.width
                itemTitle: delegateLoader.title
                itemSubtitle: delegateLoader.subtitle
                itemIconSource: delegateLoader.iconSource
                itemAlias: ""
                itemIsActive: false
                itemAccessory: delegateLoader.itemAccessory
                selected: calcHistoryView.currentIndex === delegateLoader.index
                onClicked: calcHistoryView.currentIndex = delegateLoader.index
                onActivated: calcHistoryView.itemActivated(delegateLoader.index)
            }
        }
    }
}
