import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
//import QtGraphicalEffects 1.0

Rectangle{
    width: 510
    height: 105

    property var parentRef: null

//        Image {
//            id: generalSettingExtra
//            anchors.horizontalCenter: parent.horizontalCenter
//            source: "/img/extra.png"
//        }

    Rectangle{
        id: generalSettingPanel
        width: parent.width
//            height: 115
        height: parent.height
//            anchors.top: generalSettingExtra.bottom

//        border.width: 1
//        border.color: "#6FF"

        Rectangle{
            id: generalContentPanel
            width:parent.width * 0.92
            height: parent.height * 0.66
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.04
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.13

            Column{
                width: parent.width
                height: parent.height
                spacing: 10

                Rectangle{
                    width: parent.width
                    height: 30

                    Rectangle{
                        id: filePathRow
                        width: parent.width * 0.45
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter

                        TextRow{
                            rowText: "存储路径："
                            tPlaceholderText: "/C/test";
                            tWidth: 152
                            hasButton: true
                            bButtonText: "选择"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Rectangle{
                        id: fileTypeRow
                        width: parent.width * 0.45
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left:filePathRow.right
                        anchors.leftMargin: parent.width * 0.08

                        ComboBoxRow{
                            width: parent.width
                            height: parent.height
                            labelText: "文件类型："
                            comboBoxWidth: parent.width * 0.7
                            anchors.verticalCenter: parent.verticalCenter

                            comboxModel: ['Neg']
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#9FF"
                }

                Rectangle{
                    width: parent.width
                    height: 30

                    NormalButton{
                        id: settingDefaultButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        buttonText: "恢复默认设置"
                    }

                    NormalButton{
                        id: settingSaveButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: settingCancleButton.left
                        anchors.rightMargin: 35
                        buttonText: "保 存"
                    }

                    NormalButton{
                        id: settingCancleButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        buttonText: "取 消"

                        Connections{
                            target: settingCancleButton.button
                            onClicked: {
                                if (parentRef.floatInstance !== null)
                                    parentRef.floatInstance.destroy()
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: settingRightBorder
        width: 1
        height: parent.height
        anchors.right: parent.right
        color: "#66FFFF"
    }
    Rectangle {
        id: settingLeftBorder
        width: 1
        height: parent.height
        anchors.left: parent.left
        color: "#66FFFF"
    }
    Rectangle {
        id: settingBottomBorder
        width: parent.width-2
        height: 1
        anchors.bottom: parent.bottom
        color: "#66FFFF"
    }
//    DropShadow {
//        anchors.fill: generalSettingPanel
//        horizontalOffset: 1
//        verticalOffset: 1
//        radius: 12
//        samples: 8
//        color: "#6FF"
//        source: generalSettingPanel
//    }
//    DropShadow {
//        anchors.fill: generalSettingPanel
//        horizontalOffset: -1
//        verticalOffset: -1
//        radius: 12
//        samples: 8
//        color: "#6FF"
//        source: generalSettingPanel
//    }
}

