import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Window {
    id: mainWindow
    width: 1000
    height: 560
    flags: Qt.FramelessWindowHint | Qt.Window
    visible: true

    property color textColor: "#333"
    property string textFontFamily: "宋体"
    property int textFontPixelSize: 12

    EeglabTitle{
        id: mainTitle
        target: mainWindow
    }

    Rectangle{
        id: plotControlPanel
        width: parent.width * 0.266
        height: parent.height - mainTitle.height
        anchors.top: mainTitle.bottom

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
                    font.family:"宋体"
                    font.pixelSize:12
                    color: mainWindow.textColor
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

    Rectangle{
        id: plotPanel
        width: parent.width - plotControlPanel.width
        height: parent.height - mainTitle.height
        anchors.left: plotControlPanel.right
        anchors.top: mainTitle.bottom

        Rectangle{
            id: plotAreaToolBar
            width: parent.width
            height: 40

            Rectangle{
                id: fileIconGroup
                width:parent.width * 0.195
                height: parent.height * 0.5
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.05
                anchors.verticalCenter: parent.verticalCenter
                color: "#000"

            }

            Rectangle{
                id: plotAreaToolBarSplitLine
                width: 1
                height: parent.height * 0.5
                anchors.left: fileIconGroup.right
                anchors.leftMargin: parent.width * 0.05
                anchors.verticalCenter: parent.verticalCenter
                color: "#F2F2F2"
            }

            Rectangle{
                id: deviceIconGroup
                width:parent.width * 0.252
                height: parent.height * 0.625
                anchors.left: plotAreaToolBarSplitLine.right
                anchors.leftMargin: parent.width * 0.05
                anchors.verticalCenter: parent.verticalCenter
                color: "#000"
            }

            Rectangle{
                id: plotIconGroup
                width:parent.width * 0.272
                height: parent.height * 0.625
                anchors.right: parent.right
                anchors.rightMargin: parent.width * 0.025
                anchors.verticalCenter: parent.verticalCenter
                color: "#000"
            }

            Rectangle {
                id: plotAreaToolBarBottomBorder
                width: parent.width
                height: 1
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color: "#66FFFF"
            }
        }

        Rectangle{
            id: plotArea
            width: parent.width
            height: parent.height - plotAreaToolBar.height
            anchors.top: plotAreaToolBar.bottom
        }
    }

    DeviceTestWindow{
        id:daw
        anchors.bottom: mainWindow.bottom
    }
}

