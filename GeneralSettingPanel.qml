import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtGraphicalEffects 1.0

Item {


    Rectangle{
        width: 510
        height: 129
//        color: "#00000000"

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

            Rectangle{
                id: generalContentPanel
                width:parent.width * 0.92
                height: parent.height * 0.66
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.04
                anchors.top: parent.top
                anchors.topMargin: parent.height * 0.17

                Rectangle{
                    id: filePathRow
                    width: parent.width * 0.45
                    height: 30
                    anchors.verticalCenter: parent.verticalCenter

                    TextRow{
                        rowText: "存储路径："
                        tPlaceholderText: "/C/test"
                        tWidth: 152
                        hasButton: true
                        bButtonText: "选择"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Rectangle{
                    id: fileTypeRow
                    width: parent.width * 0.45
                    height: 30
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
        }

        DropShadow {
            anchors.fill: generalSettingPanel
            horizontalOffset: 1
            verticalOffset: 1
            radius: 12
            samples: 8
            color: "#6FF"
            source: generalSettingPanel
        }
        DropShadow {
            anchors.fill: generalSettingPanel
            horizontalOffset: -1
            verticalOffset: -1
            radius: 12
            samples: 8
            color: "#6FF"
            source: generalSettingPanel
        }
    }
}

