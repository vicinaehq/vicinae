import QtQuick
import QtQuick.Controls
import QtTest
import Vicinae

Item {
    Window {
        id: owner
        width: 600
        height: 400
        x: 100
        y: 100
        visible: false

        Item {
            id: footer
            anchors.bottom: parent.bottom
            width: parent.width
            height: 48

            Item {
                id: anchor
                visible: false
                width: 200
                height: 34
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
            }
        }
    }

    FloatingControlMacOS {
        id: pill
        anchorItem: anchor
        title: "Floating control test"

        AbstractButton {
            anchors.fill: parent
            onClicked: testCase.clicks++
        }
    }

    NativeWindowProbe {
        id: probe
    }

    TestCase {
        id: testCase
        name: "FloatingControlMacOS"
        when: windowShown
        property int clicks: 0

        function expectedRect() {
            const position = anchor.mapToGlobal(0, 0);
            return Qt.rect(Math.round(position.x), Math.round(position.y), Math.round(anchor.width), Math.round(anchor.height));
        }

        function verifyGeometry() {
            const rect = expectedRect();
            tryCompare(pill, "x", rect.x);
            tryCompare(pill, "y", rect.y);
            tryCompare(pill, "width", rect.width);
            tryCompare(pill, "height", rect.height);
            compare(pill.minimumWidth, rect.width);
            compare(pill.maximumWidth, rect.width);
            compare(pill.minimumHeight, rect.height);
            compare(pill.maximumHeight, rect.height);
        }

        function init() {
            failOnWarning(/.*/);
            probe.stop();
            clicks = 0;
            anchor.visible = false;
            owner.visible = true;
            owner.x = 100;
            owner.y = 100;
            owner.height = 400;
            anchor.width = 200;
            anchor.height = 34;
            waitForRendering(owner.contentItem);
            anchor.visible = true;
            tryCompare(pill, "visible", true);
            verifyGeometry();
            waitForRendering(pill.contentItem);
        }

        function cleanup() {
            probe.stop();
        }

        function test_rightEdgeStaysFixedDuringResize() {
            const rightEdge = pill.x + pill.width;
            probe.start(pill);
            for (const width of [340, 120, 280, 200]) {
                anchor.width = width;
                verifyGeometry();
            }
            verify(probe.frames.length > 0);
            for (const frame of probe.frames)
                compare(frame.x + frame.width, rightEdge, "Resize must not expose the new width at the old position");
        }

        function test_layoutChangesAreCoalesced() {
            probe.start(pill);
            anchor.width = 350;
            anchor.width = 80;
            anchor.width = 260;
            verifyGeometry();
            verify(probe.frames.length > 0);
            for (const frame of probe.frames)
                compare(frame.width, 260, "Intermediate layout widths should not resize the native window");
        }

        function test_ownerMoveResizeAndReshow() {
            owner.x += 80;
            owner.y += 40;
            owner.height += 60;
            verifyGeometry();
            owner.visible = false;
            tryCompare(pill, "visible", false);
            anchor.width = 300;
            owner.height -= 90;
            owner.visible = true;
            tryCompare(pill, "visible", true);
            verifyGeometry();
        }

        function test_edgesStayClickableAfterResize() {
            anchor.width = 320;
            verifyGeometry();
            waitForRendering(pill.contentItem);
            mouseClick(pill.contentItem, 2, pill.height / 2);
            mouseClick(pill.contentItem, pill.width - 2, pill.height / 2);
            compare(clicks, 2);
        }
    }
}
