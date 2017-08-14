import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
//import QtGraphicalEffects 1.0

Rectangle{
    width: 510
    height: 308
    color: "#FFF"

    property var parentRef: null

    Rectangle{
        id: generalSettingPanel
        width: parent.width
        height: parent.height

//        border.width: 1
//        border.color: "#6FF"

        Rectangle {
            id: gatherRightBorder
            width: 1
            height: parent.height
            anchors.right: parent.right
            color: "#66FFFF"
        }
        Rectangle {
            id: gatherLeftBorder
            width: 1
            height: parent.height
            anchors.left: parent.left
            color: "#66FFFF"
        }
        Rectangle {
            id: gatherBottomBorder
            width: parent.width-2
            height: 1
            anchors.bottom: parent.bottom
            color: "#66FFFF"
        }

        Rectangle{
            id: generalContentPanel
            width:parent.width * 0.92
            height: parent.height * 0.82
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.04
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.05

            Column{
                width: parent.width
                height: parent.height
                spacing: 12

                Rectangle{
                    width: parent.width
                    height: inforGrid.height + remarkInputArea.height

                    Grid{
                        id: inforGrid
                        rows: 5
                        columns: 2
                        rowSpacing: 18
                        columnSpacing: 27

                        TextRow{
                            rowText: "索引："
                            tWidth: 174
                            tPlaceholderText: "请输入.."
                        }

                        Rectangle{
                            id: userSex
                            width: 174
                            height: 25
                            Label{
                                id: userSexLabel
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                font.family:"宋体"
                                font.pixelSize:12
                                color: "#333"
                                text: "性别："
                            }

                            ExclusiveGroup { id: sexOption }
                            NormalCheckbox{
                                id: sexOptionMale
                                anchors.left: userSexLabel.right
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                checkboxText: "男"
                                cExclusiveGroup: sexOption
                            }

                            NormalCheckbox{
                                id: sexOptionFemale
                                anchors.left: sexOptionMale.right
                                anchors.leftMargin: parent.width / 3
                                anchors.verticalCenter: parent.verticalCenter
                                checkboxText: "女"
                                cExclusiveGroup: sexOption
                            }
                        }

                        TextRow{
                            rowText: "姓名："
                            tWidth: 174
                            tPlaceholderText: "请输入.."
                        }
                        TextRow{
                            rowText: "年龄："
                            tWidth: 174
                            tPlaceholderText: "请输入.."
                        }
                        TextRow{
                            rowText: "操作员："
                            tWidth: 162
                            tPlaceholderText: "请输入.."
                        }
                        TextRow{
                            rowText: "单位："
                            tWidth: 174
                            tPlaceholderText: "请输入.."
                        }
                        TextRow{
                            rowText: "采集时间:"
                            tWidth: 156
                            tPlaceholderText: "请输入.."
                        }
                        ComboBoxRow{
                            width: 174
                            height: 25
                            labelText: "串口："
                            comboBoxWidth: 174

                            comboxModel: ['COM1', 'COM2', 'COM3']
                        }
                    }

                    TextRow{
                        id: remarkInputArea
                        rowText: "备注："
                        tWidth: 422
                        tHeight: 55
                        tPlaceholderText: "请输入.."
                        anchors.top: inforGrid.bottom
                        anchors.topMargin: 0
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
                        id: gatherInputDefaultButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        buttonText: "恢复默认设置"
                    }

                    NormalButton{
                        id: gatherInputSaveButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: gatherInputCancleButton.left
                        anchors.rightMargin: 35
                        buttonText: "保 存"
                    }

                    NormalButton{
                        id: gatherInputCancleButton
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        buttonText: "取 消"

                        Connections{
                            target: gatherInputCancleButton.button
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

