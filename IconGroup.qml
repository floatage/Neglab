import QtQuick 2.0

Rectangle{
    property color hoveColor : "#6FF"
    property color clickColor : "#6CF"
    property var iconDataMap: []

    Rectangle{
        id: toolIconGroup

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
}

