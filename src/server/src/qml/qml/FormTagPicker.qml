import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root
  readonly property real _minHeight: 28
  readonly property real _maxHeight: 360
  readonly property real _contentPadding: 5
  readonly property real _contentHeight: flow.implicitHeight + _contentPadding * 2

  implicitHeight: Math.max(_minHeight, Math.min(_contentHeight, _maxHeight))
  Layout.minimumHeight: _minHeight
  Layout.maximumHeight: _maxHeight
  Layout.preferredHeight: implicitHeight
  Layout.fillWidth: true
  activeFocusOnTab: true

  property var items: []
  property list<string> pickedItems: []
  property var availableItems: items.filter((item) => !(pickedItems.includes(item.id)))
  property var _pendingFocusRequest: null
  property alias text: textInput.text
  property var fuzzySearchResults: []
  property alias cursorPosition: textInput.cursorPosition
  
  signal pickedItemsEdited(var items)
  signal textEdited()

  function forceActiveFocus() { textInput.forceActiveFocus() }
  function selectAll() { textInput.selectAll() }
  function _queueFocusAfterRemove(removedIndex) {
    if (removedIndex > 0) {
      _pendingFocusRequest = {
        type: "chip",
        chipId: root.pickedItems[removedIndex - 1]
      }
      return
    }

    _pendingFocusRequest = { type: "input" }
  }

  function _applyPendingFocus() {
    const request = _pendingFocusRequest
    _pendingFocusRequest = null
    if (!request) return

    if (request.type === "input" || repeater.count === 0) {
      textInput.forceActiveFocus()
      return
    }

    for (let i = 0; i < repeater.count; ++i) {
      const chip = repeater.itemAt(i)
      if (chip && chip.chipId === request.chipId) {
        chip.forceActiveFocus()
        return
      }
    }

    textInput.forceActiveFocus()
  }

  function _updateFuzzySearchResults(searchText) {
      root.fuzzySearchResults = TagSearch.search(searchText, root.availableItems);
  }


  onActiveFocusChanged: {
    if (activeFocus) textInput.forceActiveFocus()
  }

  onPickedItemsChanged: {
    if (_pendingFocusRequest !== null) {
      Qt.callLater(root._applyPendingFocus)
    }
  }

  Rectangle {
    id: border
    anchors.fill: parent
    radius: 8
    color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                       Theme.secondaryBackground.b, Config.windowOpacity)
    border.color: textInput.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
    border.width: 1

    Flow {
      id: flow
      anchors {
        fill: parent
        margins: 5
        rightMargin: 10
        leftMargin: 10
      }
      spacing: 4

      Repeater {
        anchors.verticalCenter: parent.verticalCenter
        id: repeater
        model: root.pickedItems

        TagPickerTag {
          required property int index
          required property string modelData
          property var chip: items.find((item) => item.id === modelData) || ({
            id: modelData,
            displayName: modelData,
            iconSource: ""
          })

          chipId: chip.id
          text: chip.displayName
          imageSource: chip.iconSource

          onRemoveRequested: {
            const copy = root.pickedItems.slice()
            root._queueFocusAfterRemove(index)
            copy.splice(index, 1)
            root.pickedItemsEdited(copy)
          }

          onFocusOnPreviousChipRequested: {
            if (index > 0) {
              repeater.itemAt(index - 1).forceActiveFocus()
            } else {
              textInput.forceActiveFocus()
            }
          }

          onFocusOnNextChipRequested: {
            if (index < repeater.count - 1) {
              repeater.itemAt(index + 1).forceActiveFocus()
            } else {
              textInput.forceActiveFocus()
            }
          }
        }
      }

      TextInput {
        id: textInput
        verticalAlignment: TextInput.AlignVCenter
        font.pointSize: Theme.regularFontSize
        color: Theme.foreground
        selectionColor: Theme.textSelectionBg
        selectedTextColor: Theme.textSelectionFg
        clip: true
        height: Math.max(contentHeight, 28)
        width: Math.min(Math.max(contentWidth + 4, 80), flow.width)

        onTextEdited: {
          root.textEdited()
          root._updateFuzzySearchResults(textInput.text)
        }

        Keys.onPressed: (event) => {
          if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            if (textInput.text.length == 0 || availableItems.length == 0) {
              event.accepted = false
              return
            }
            var copy = pickedItems.slice()
            var selectedId = root.fuzzySearchResults.length > 0 ? root.fuzzySearchResults[0].id : null; 
            if (!selectedId) {
              event.accepted = false
              return
            }
            if (copy.includes(selectedId)) {
              event.accepted = false
              return
            }

            copy.push(selectedId)
            pickedItemsEdited(copy)
            textInput.text = ""
            event.accepted = true
          } else if (event.key === Qt.Key_Backspace
            && text === ""
            && root.pickedItems.length > 0
          ) {
            repeater.itemAt(root.pickedItems.length - 1).forceActiveFocus()
            event.accepted = true
          } else if (event.key === Qt.Key_Left
            && cursorPosition === 0 
            && root.pickedItems.length > 0
          ) {
            repeater.itemAt(root.pickedItems.length - 1).forceActiveFocus()
            event.accepted = true
          }
        }
      }
    }
  }

  Popup {
    id: fuzzySearchPopup
    parent: border
    closePolicy: Popup.NoAutoClose
    visible: textInput.text.length > 0 && root.fuzzySearchResults.length > 0

    x: 0
    y: border.height + 4
    width: border.width
    height: Math.min(fuzzySearchResults.length * 30, 180)

    focus: false
    modal: false
    Keys.enabled: false

    background: Rectangle {
      radius: 8
      color: Theme.background
      border {
        color: Theme.divider
        width: 1
      }
    }

    contentItem: ListView {
      id: fuzzyResultList
      spacing: 4
      anchors.fill: parent
      model: root.fuzzySearchResults
      clip: true
      reuseItems: true
      boundsBehavior: Flickable.StopAtBounds

      delegate: Item {
        width: fuzzyResultList.width
        height: 30

        Rectangle {
          anchors.fill: parent
          anchors.leftMargin: 2
          anchors.rightMargin: 2
          radius: 6
          color: itemHover.hovered ? Theme.listItemHoverBg : "transparent"
        }

        RowLayout {
          anchors.fill: parent
          anchors.leftMargin: 8
          anchors.rightMargin: 8
          spacing: 4

          Loader {
            active: modelData.iconSource !== undefined && modelData.iconSource !== ""
            Layout.alignment: Qt.AlignVCenter

            sourceComponent: Component {
              ViciImage {
                width: 20
                height: 20
                source: modelData.iconSource
              }
            }
          }

          Text {
            text: modelData.displayName
            color: Theme.foreground
            font.pointSize: Theme.smallerFontSize
            elide: Text.ElideRight
            Layout.fillWidth: true
          }
        }

        HoverHandler {
          id: itemHover
          cursorShape: Qt.PointingHandCursor
        }

        TapHandler {
          gesturePolicy: TapHandler.ReleaseWithinBounds
          onTapped: {
            var copy = pickedItems.slice()
            copy.push(modelData.id)
            pickedItemsEdited(copy)
            textInput.text = ""
          }
        }
      }
    }
  }
}
