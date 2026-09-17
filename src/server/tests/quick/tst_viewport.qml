import QtQuick
import QtQuick.Controls
import QtTest
import Vicinae

Item {
    width: 500
    height: 480

    LauncherWindow {
        id: window
        width: 500
        height: 480
        visible: true
        property real headerInset: 60

        StackView {
            id: stack
            anchors.fill: parent
        }
    }

    Component {
        id: pageComponent
        LauncherPage {
            headerInset: window.headerInset
        }
    }

    TestCase {
        name: "LauncherViewport"
        when: windowShown

        function init() {
            failOnWarning(/.*/);
            window.headerInset = 60;
            window.statusBarOverlap = 48;
            window.visible = true;
        }

        function cleanup() {
            stack.clear(StackView.Immediate);
        }

        function push(file = "ViewportPage.qml", properties = {}) {
            const page = stack.push(pageComponent, {
                viewUrl: Qt.resolvedUrl("fixtures/" + file),
                viewProperties: properties
            }, StackView.Immediate);
            verify(page.view !== null);
            waitForRendering(page.view);
            return page;
        }

        function test_initialPlacement_data() {
            return [
                {
                    tag: "overlay",
                    inset: 60
                },
                {
                    tag: "reserved",
                    inset: 0
                }
            ];
        }

        function test_initialPlacement(data) {
            window.headerInset = data.inset;
            const page = push();
            const viewport = page.view.viewport;
            compare(page.view.mapToItem(page, 0, 0).y, data.inset);
            compare(page.view.height, window.height - data.inset);
            compare(page.view.preview.mapToItem(null, 0, 0).y, data.inset);
            compare(viewport.topInset, data.inset);
            compare(viewport.flickable.mapToItem(null, 0, 0).y, 0);
            tryCompare(viewport.flickable, "contentY", -data.inset - 4);
            compare(viewport.flickable.ScrollBar.vertical.topPadding, data.inset);
            compare(viewport.bottomInset, 72);
        }

        function test_scrollsBehindHeader() {
            const viewport = push().view.viewport;
            viewport.scrollTo(viewport.minimumY + 40);
            const contentTop = viewport.flickable.contentItem.mapToItem(null, 0, 0).y;
            compare(contentTop, 24);
            verify(contentTop < window.headerInset);
            const pixels = grabImage(viewport.flickable);
            const scale = pixels.height / viewport.flickable.height;
            verify(pixels.pixel(20 * scale, 30 * scale).r > 0.9);
        }

        function test_independentPanes() {
            const view = push().view;
            compare(view.rightViewport.topInset, 60);
            const original = view.rightViewport.flickable.contentY;
            view.viewport.scrollTo(400);
            compare(view.rightViewport.flickable.contentY, original);
            compare(view.preview.mapToItem(null, 0, 0).y, 60);
        }

        function test_fixedToolbarAndNestedScroller() {
            const view = push("ViewportPage.qml", {
                toolbarHeight: 42
            }).view;
            compare(view.viewport.topInset, 0);
            compare(view.viewport.flickable.mapToItem(null, 0, 0).y, 102);
            compare(view.nested.topInset, 0);
            view.viewport.scrollTo(200);
            compare(view.nested.topInset, 0);
            view.toolbarHeight = 0;
            compare(view.viewport.topInset, 60);
        }

        function test_headingAndSelectedRow_data() {
            return [
                {
                    tag: "overlay",
                    inset: 60
                },
                {
                    tag: "reserved",
                    inset: 0
                }
            ];
        }

        function test_headingAndSelectedRow(data) {
            window.headerInset = data.inset;
            const viewport = push("ListPage.qml").view;
            const list = viewport.flickable;
            viewport.revealIndex(120, ListView.Beginning);
            viewport.revealIndex(114, ListView.Contain, 115);
            const heading = list.itemAtIndex(114);
            const row = list.itemAtIndex(115);
            verify(heading !== null && row !== null);
            verify(heading.y >= list.contentY + viewport.topInset);
            verify(row.y + row.height <= list.contentY + list.height - viewport.bottomInset);
            viewport.revealIndex(0, ListView.Beginning, 1);
            verify(list.itemAtIndex(0).y >= list.contentY + viewport.topInset);
        }

        function test_lastRowClearsFooter() {
            const viewport = push("ListPage.qml").view;
            const list = viewport.flickable;
            viewport.revealIndex(199, ListView.End);
            const row = list.itemAtIndex(199);
            verify(row !== null);
            verify(row.y + row.height <= list.contentY + list.height - viewport.bottomInset);
            verify(list.contentY <= viewport.maximumY);
        }

        function test_initialSelectionIsPreserved_data() {
            return test_initialPlacement_data();
        }

        function test_initialListShowsHeading() {
            const viewport = push("ListPage.qml").view;
            const list = viewport.flickable;
            tryCompare(list, "contentY", viewport.minimumY);
            verify(list.itemAtIndex(0).y >= list.contentY + viewport.topInset);
        }

        function test_initialSelectionIsPreserved(data) {
            window.headerInset = data.inset;
            const viewport = push("ListPage.qml", {
                initialIndex: 80
            }).view;
            verify(viewport.isIndexVisible(80));
            verify(viewport.flickable.contentY > 200);
        }

        function test_shortContentAndBounds() {
            const viewport = push().view.viewport;
            viewport.flickable.contentHeight = 80;
            compare(viewport.scrollable, false);
            viewport.scrollTo(500);
            compare(viewport.flickable.contentY, viewport.minimumY);
            viewport.revealRect(0, 40);
            compare(viewport.flickable.contentY, viewport.minimumY);
        }

        function test_navigationRestoresPosition() {
            const first = push();
            const viewport = first.view.viewport;
            viewport.scrollTo(417);
            const originalY = viewport.flickable.contentY;
            for (let i = 0; i < 5; i++) {
                // The navigation controller changes the header before pushing,
                // but restores it after popping the QML page.
                window.headerInset = 0;
                push("ListPage.qml");
                compare(first.topInset, 60);
                compare(viewport.flickable.contentY, originalY);
                stack.pop(StackView.Immediate);
                window.headerInset = 60;
                waitForRendering(first);
                compare(stack.currentItem, first);
                compare(viewport.flickable.contentY, originalY);
            }
            compare(first.view.activations, 6);
            window.visible = false;
            window.visible = true;
            compare(viewport.flickable.contentY, originalY);
        }

        function test_footerTracksAncestorLayout() {
            const view = push().view;
            compare(view.viewport.bottomInset, 72);
            view.height -= 100;
            compare(view.viewport.bottomInset, 0);
            view.y += 100;
            compare(view.viewport.bottomInset, 72);
        }

        function test_formFocusClearsChrome_data() {
            return test_initialPlacement_data();
        }

        function test_formFocusClearsChrome(data) {
            window.headerInset = data.inset;
            const form = push("FormPage.qml").view;
            const last = form.fields.itemAt(19);
            window.requestActivate();
            last.forceActiveFocus();
            tryCompare(last, "activeFocus", true);
            verify(last.mapToItem(null, 0, last.height).y <= window.statusBarTop - 24);
            form.focusFirst();
            const first = form.fields.itemAt(0);
            tryCompare(first, "activeFocus", true);
            verify(first.mapToItem(null, 0, 0).y >= window.headerInset);
        }

        function test_textMatchPosition_data() {
            return test_initialPlacement_data();
        }

        function test_textMatchPosition(data) {
            window.headerInset = data.inset;
            const page = stack.push(pageComponent, {
                viewUrl: "qrc:/qt/qml/Vicinae/TextViewer.qml",
                viewProperties: {
                    text: "Before\n".repeat(40) + "Needle\n" + "After\n".repeat(40),
                    highlightTerms: ["Needle"]
                }
            }, StackView.Immediate);
            verify(page.view !== null);
            waitForRendering(page.view);
            const viewer = page.view;
            verify(viewer.flickable.contentY > 200);
            compare(viewer.verticalScrollBar.topPadding, data.inset);
            viewer.highlightTerms = [];
            tryCompare(viewer.flickable, "contentY", viewer.minimumY);
        }

        function test_scriptOutputBoundsAndFocus_data() {
            return test_initialPlacement_data();
        }

        function test_scriptOutputBoundsAndFocus(data) {
            window.headerInset = data.inset;
            const page = stack.push(pageComponent, {
                viewUrl: "qrc:/qt/qml/Vicinae/ScriptOutputText.qml",
                viewProperties: {
                    text: "Output<br>".repeat(100)
                }
            }, StackView.Immediate);
            verify(page.view !== null);
            waitForRendering(page.view);
            const output = page.view;
            verify(output.contentHeight > 1000);
            compare(output.verticalScrollBar.topPadding, data.inset);
            output.scrollToBottom();
            compare(output.flick.contentY, output.maximumY);
            output.moveUp();
            compare(output.flick.contentY, output.maximumY - 40);
            output.text = "Short";
            waitForRendering(output);
            output.moveDown();
            compare(output.flick.contentY, output.minimumY);
            window.requestActivate();
            output.focusText();
            verify(window.activeFocusItem !== null);
        }
    }
}
