pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

QtObject {
    id: root
    required property DocumentView view
    property bool following: true
    property ScrollBar scrollbar: null
    readonly property bool scrollbarPressed: scrollbar?.pressed ?? false
    readonly property bool paused: scrollbarPressed || view.document.selecting
    readonly property bool atEnd: view.atYEnd
    readonly property bool showJumpButton: !following && view.maximumY - view.contentY > 40
    property var _resumePosition: null
    property real _scrollbarRange: 0

    function pauseFollowing() {
        following = false;
        _resumePosition = null;
    }

    function scrollToBottom() {
        _resumePosition = null;
        following = true;
        view.scrollToEnd();
    }

    function positionChanged() {
        if (following || paused || view.layoutChanging)
            return;
        if (_resumePosition !== null && (atEnd || view.contentY >= _resumePosition - 2))
            scrollToBottom();
    }

    function handleWheel(wheel) {
        if (wheel.modifiers & Qt.AltModifier)
            return;
        const delta = wheel.angleDelta.x !== 0 || wheel.angleDelta.y !== 0 ? wheel.angleDelta.y : wheel.pixelDelta.y;
        if (delta > 0) {
            pauseFollowing();
        } else if (delta < 0) {
            if (following && !paused) {
                wheel.accepted = true;
                view.scrollToEnd();
            } else {
                // Keep this gesture's destination even if generation extends the document meanwhile.
                _resumePosition = view.maximumY;
                positionChanged();
            }
        }
    }

    function scrollBy(distance) {
        pauseFollowing();
        if (distance > 0)
            _resumePosition = view.maximumY;
        view.contentY = Math.max(view.minimumY, Math.min(view.maximumY, view.contentY + distance));
        positionChanged();
    }

    function scrollbarMoved() {
        if (!scrollbarPressed)
            return;
        const range = view.maximumY - view.minimumY;
        const rangeChanged = Math.abs(range - _scrollbarRange) > 0.01;
        _scrollbarRange = range;
        // Qt updates the thumb position before its size when the scroll range changes.
        if (view.layoutChanging || rangeChanged)
            return;
        following = scrollbar.position + scrollbar.size >= 1 - 0.00001;
    }

    function selectionChanged() {
        if (view.document.selecting || view.document.hasSelection)
            pauseFollowing();
        else if (atEnd)
            scrollToBottom();
    }

    onScrollbarPressedChanged: {
        if (scrollbarPressed) {
            _scrollbarRange = view.maximumY - view.minimumY;
            pauseFollowing();
        } else if (following || atEnd)
            scrollToBottom();
    }

    property Connections _scrollbarConnection: Connections {
        target: root.scrollbar
        function onPositionChanged() {
            root.scrollbarMoved();
        }
    }

    property Connections _positionConnection: Connections {
        target: root.view
        function onContentYChanged() {
            root.positionChanged();
        }
        function onLayoutUpdated() {
            if (!root.following)
                root.positionChanged();
        }
        function onMovementStarted() {
            if (root.view.dragging || root.view.flicking) {
                root.pauseFollowing();
                root._resumePosition = root.view.maximumY;
            }
        }
    }

    property Connections _selectionConnection: Connections {
        target: root.view.document
        function onSelectingChanged() {
            root.selectionChanged();
        }
        function onHasSelectionChanged() {
            root.selectionChanged();
        }
    }
}
