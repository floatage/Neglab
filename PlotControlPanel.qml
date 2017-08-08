import QtQuick 2.0
import QtQuick.Controls 1.3

Rectangle{
    property color textColor: "#333"
    property string textFontFamily: "宋体"
    property int textFontPixelSize: 12

    Column{
        width: parent.width * 0.9
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: parent.width * 0.05

        Rectangle{
            id: plotModeRow
            width: parent.width
            height: parent.height / 7
            Label{
                id: plotModeRowLabel
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                font.family:textFontFamily
                font.pixelSize:textFontPixelSize
                color: textColor
                text: "显示模式"
            }

            ExclusiveGroup { id: plotModeGroup }
            NormalCheckbox{
                id: plotModeContrast
                anchors.left: plotModeRowLabel.right
                anchors.leftMargin: parent.width * 0.06
                anchors.verticalCenter: parent.verticalCenter
                checkboxText: "对比"
                cExclusiveGroup: plotModeGroup
            }

            NormalCheckbox{
                id: plotModeSplit
                anchors.left: plotModeContrast.right
                anchors.leftMargin: parent.width / 6
                anchors.verticalCenter: parent.verticalCenter
                checkboxText: "分离"
                cExclusiveGroup: plotModeGroup
            }
        }

        Rectangle{
            width: parent.width
            height: parent.height / 7

            SliderRow{
                rowWidth: parent.width
                rowHeight: parent.height
                labelText: "时间间隔"
                sliderText: "20.0"
                sliderWidth: parent.width * 0.6
            }
        }

        Rectangle{
            id: eegMaxRow
            width: parent.width
            height: parent.height / 7

            TextRow{
                anchors.verticalCenter: parent.verticalCenter
                rowText: "脑电上限"
                tWidth: 174
                tPlaceholderText: "6.600"
            }
        }

        Rectangle{
            id: eegMinRow
            width: parent.width
            height: parent.height / 7

            TextRow{
                anchors.verticalCenter: parent.verticalCenter
                rowText: "脑电下限"
                tWidth: 174
                tPlaceholderText: "-6.600"
            }
        }

        Rectangle{
            id: highpassFilterRow
            width: parent.width
            height: parent.height / 7
            OptionalTextRow{
                rowWidth: parent.width
                rowHeight: parent.height
                checkboxText: "高通滤波"
                placeholderText: "30.00"
                textWidth: parent.width * 0.6
            }
        }

        Rectangle{
            id: lowpassFilterRow
            width: parent.width
            height: parent.height / 7
            OptionalTextRow{
                rowWidth: parent.width
                rowHeight: parent.height
                checkboxText: "低通滤波";
                placeholderText: "30.00"
                textWidth: parent.width * 0.6
            }
        }

        Rectangle{
            id: sampleRateRow
            width: parent.width
            height: parent.height / 7
            OptionalTextRow{
                rowWidth: parent.width
                rowHeight: parent.height
                checkboxText: "采样率"
                placeholderText: "80.0"
                textWidth: parent.width * 0.65
            }
        }
    }

    Rectangle {
        id: plotControlPanelRightBorder
        width: 1
        height: parent.height
        anchors.right: parent.right
        color: "#66FFFF"
    }
}

