import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
//import QtGraphicalEffects 1.0

Rectangle{
    width: 510
    height: 295
    color: "#FFF"

    Rectangle{
        id: generalSettingPanel
        width: parent.width
        height: parent.height

        border.width: 1
        border.color: "#6FF"

        Rectangle{
            id: generalContentPanel
            width:parent.width * 0.92
            height: parent.height * 0.82
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.04
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.09

            Grid{
                id: inforGrid
                rows: 5
                columns: 2
                rowSpacing: 22
                columnSpacing: 20

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
                rowText: "备注："
                tWidth: 415
                tHeight: 55
                tPlaceholderText: "请输入.."
                anchors.top: inforGrid.bottom
                anchors.topMargin: 0
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

