import QtQuick 2.0

Rectangle {
    id: titleArea
    width: parent.width
    height: 40
    anchors.left: parent.left
    anchors.top: parent.top
    color: "#003333"

    property alias iconPath: iconPic.source
    property color sleepColor: "#58F"
    property color activeColor: "#6CF"
    property var target: null

    Component.onCompleted: {
        titleArea.iconPath = "/img/logo.png"
    }

    Rectangle{
        width: parent.width * 0.97
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.015
        anchors.rightMargin: parent.width * 0.015
        color: parent.color

        Rectangle {
            id: iconArea
            width: parent.width * 0.1
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: iconPic
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle{
            id: toolIconGroup
            width: parent.width * 0.107
            height: parent.height * 0.425
            anchors.right: titleIconSplitLine.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: parent.width * 0.031
            color: parent.color

            Rectangle {
                id:bluetoothIcon
                width: parent.width * 0.2
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/bluetooth.png"
                }
            }

            Rectangle {
                id: styleIcon
                width: parent.width * 0.2
                anchors.left: bluetoothIcon.right
                anchors.leftMargin: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/style.png"
                }
            }

            Rectangle {
                id: settingIcon
                width: parent.width * 0.2
                anchors.left:styleIcon.right
                anchors.leftMargin: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/setting.png"
                    scale: 0.7
                }
            }
        }

        Rectangle {
            id: titleIconSplitLine
            width: 1
            height: parent.height * 0.3
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: windowControlIconGroup.left
            anchors.rightMargin: parent.width * 0.031
            color: "#868686"
        }

        Rectangle{
            id: windowControlIconGroup
            width: parent.width * 0.088
            height: parent.height * 0.3
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            color: parent.color

            Rectangle {
                id:minIcon
                width: parent.width * 0.2
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/min.png"
                }
            }

            Rectangle {
                id: maxIcon
                width: parent.width * 0.2
                anchors.left: minIcon.right
                anchors.leftMargin: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/max.png"
                }
            }

            Rectangle {
                id: closeIcon
                width: parent.width * 0.2
                anchors.left:maxIcon.right
                anchors.leftMargin: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "/img/close.png"
                }
            }
        }
    }

    MouseArea {
        property bool isPress: false
        property int mousePosPreX
        property int mousePosPreY

        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true

        onDoubleClicked: {
            Qt.quit();
        }

        onPressed: {
            isPress = true
            mousePosPreX = mouseX
            mousePosPreY = mouseY
        }

        onReleased: {
            isPress = false
        }

        onPositionChanged: {
            if (isPress == true){
                target.x += mouseX - mousePosPreX
                target.y += mouseY - mousePosPreY
            }
        }
    }
}


