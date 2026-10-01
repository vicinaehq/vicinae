pragma ComponentBehavior: Bound
import QtQuick

ScrollViewport {
    id: root
    required property ListView list
    flickable: root.list

    // Without an overlay, preserve the ListView's native initial positioning.
    resetOnInitialization: topInset > 0

    function resetPosition() {
        list.forceLayout();
        scrollTo(minimumY);
    }

    function isIndexVisible(index: int): bool {
        const item = list.itemAtIndex(index);
        return item !== null && item !== undefined && item.y + item.height > list.contentY + topInset && item.y < list.contentY + list.height - bottomInset;
    }

    // The model supplies the range (e.g. heading + first row); geometry stays here.
    function revealIndex(index, mode = ListView.Contain, endIndex = index) {
        if (index < 0)
            return;
        _positioned = true;
        list.positionViewAtIndex(index, mode);
        const first = list.itemAtIndex(index);
        const last = list.itemAtIndex(endIndex) ?? first;
        if (first && last)
            revealRect(first.y, last.y + last.height);
    }
}
