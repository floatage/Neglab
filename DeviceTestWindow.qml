import QtQuick 2.0

Rectangle{
    id: deviceTestDialog
    width: 510
    height: 315

    Rectangle {
        id: deviceTestTitleArea
        width: parent.width
        height: 35
        anchors.left: parent.left
        anchors.top: parent.top
        color: "#003333"

        property alias iconPath: iconPic.source
        property color sleepColor: "#58F"
        property color activeColor: "#6CF"

        Component.onCompleted: {
            deviceTestTitleArea.iconPath = "/img/logo.png"
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
                width: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    id: iconPic
                    anchors.verticalCenter: parent.verticalCenter
                    scale: 0.875
                }
            }

            Rectangle{
                id: windowControlIconGroup
                width: parent.width * 0.15
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
                        scale: 0.875
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
                        scale: 0.875
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
                        scale: 0.875
                    }
                }
            }
        }

        MouseArea {
            property bool isPress: false
            property int mousePosPreX
            property int mousePosPreY

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            propagateComposedEvents: true

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
                    deviceTestDialog.x += mouseX - mousePosPreX
                    deviceTestDialog.y += mouseY - mousePosPreY
                }
            }
        }
    }

    Rectangle{
        width: parent.width
        height: parent.height - deviceTestTitleArea.height
        anchors.top: deviceTestTitleArea.bottom
        border.width: 1
        border.color: "#6FF"

        Rectangle{
            width: parent.width * 0.94
            height: parent.height * 0.9
            anchors.centerIn: parent

            Column {
                width: parent.width
                height: parent.height
                spacing: 12

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.6
                    border.width: 1
                    border.color: "#6FF"
                }

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.18

                    TextRow{
                        tWidth: parent.width
                        tHeight: parent.height
                        tPlaceholderText: "请输入.."
                    }
                }

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.12

                    Row{
                        width: parent.width * 0.7
                        height: parent.height
                        spacing: 25

                        NormalButton{
                            id: deviceSearchButton
                            buttonText: "搜 索"
                            anchors.bottom: parent.bottom
                        }

                        Rectangle{
                            id: deviceListCombox
                            width: 90
                            height: parent.height
                            anchors.verticalCenter: parent.verticalCenter

                            ComboBoxRow{
                                width: 90
                                height: 30
                                comboBoxWidth: 90
                                comboBoxHeight: 30
                                comboxModel: ['COM2', 'COM3', 'COM4']
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.leftMargin: -10
                            }
                        }

                        NormalButton{
                            id: deviceConnectButton
                            buttonText: "连 接"
                            anchors.bottom: parent.bottom
                        }

                        NormalButton{
                            id: deviceDisconnectButton
                            buttonText: "断 开"
                            anchors.bottom: parent.bottom
                        }
                    }

                    NormalButton{
                        id: msgSendButton
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        buttonText: "发 送"
                    }
                }
            }
        }
    }
}

