import QtQuick
import QtQuick.Controls
import Odizinne.FramelessWindow

ApplicationWindow {
    id: root

    property Component titleBar: DefaultTitleBar {}
    readonly property bool maximized: visibility === Window.Maximized

    visible: false

    Component.onCompleted: {
        FramelessHelper.setup(root, _titleBarLoader.height)
        root.show()
    }

    Loader {
        id: _titleBarLoader
        anchors { top: parent.top; left: parent.left; right: parent.right }
        sourceComponent: root.titleBar
        onHeightChanged: FramelessHelper.setCaptionHeight(height)
    }

    component DefaultTitleBar: Rectangle {
        height: 32
        color: "#313244"

        Text {
            anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
            text: root.title
            color: "#cdd6f4"
            font.pixelSize: 13
        }

        Row {
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }

            TitleBarButton {
                text: "─"
                onClicked: root.showMinimized()
            }
            TitleBarButton {
                text: root.maximized ? "❐" : "□"
                onClicked: root.maximized ? root.showNormal() : root.showMaximized()
            }
            TitleBarButton {
                text: "✕"
                hoverColor: "#c0392b"
                onClicked: root.close()
            }
        }
    }

    component TitleBarButton: Rectangle {
        property alias text: label.text
        property color hoverColor: "#45475a"
        signal clicked

        width: 46
        height: 32
        color: area.containsMouse ? hoverColor : "transparent"

        Text {
            id: label
            anchors.centerIn: parent
            color: "#cdd6f4"
            font.pixelSize: 13
        }
        MouseArea {
            id: area
            anchors.fill: parent
            hoverEnabled: true
            onClicked: parent.clicked()
        }
    }
}