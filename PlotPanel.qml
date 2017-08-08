import QtQuick 2.0
import QtQuick.Controls 1.3

Rectangle{

    Component {
        id: iconItem

        Rectangle{
            id: iconRect
            width: iconLen
            height: iconLen

            property real iconLen : 20
            property string imgSource
            property real imgScale: 1.0

            signal picLoad(Image img)
            signal iconClicked()

            Image {
                id:iconPic
                source: imgSource
                scale: imgScale
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                onClicked: iconRect.iconClicked()
            }
        }
    }

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

            Row{
                spacing: 45

                Loader {
                    id: creatIcon
                    sourceComponent: iconItem
                    onLoaded: item.imgSource = "/img/creat.png"
                }

                Loader {
                    id: saveIcon
                    sourceComponent: iconItem
                    onLoaded: item.imgSource = "/img/save.png"
                }

                Loader {
                    id: openIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/open.png"
                    }
                }
            }
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
            anchors.leftMargin: parent.width * 0.045
            anchors.verticalCenter: parent.verticalCenter

            Row{
                spacing: 45
                anchors.verticalCenter: parent.verticalCenter

                Loader {
                    id: inforIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/userinfor.png"
                        item.imgScale = 0.85
                    }
                }

                Loader {
                    id: deviceStartIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/start.png"
                        item.imgScale = 0.85
                    }
                }

                Loader {
                    id: devicePauseIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/pause.png"
                        item.imgScale = 0.85
                    }
                }

                Loader {
                    id: deviceStopIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/stop.png"
                        item.imgScale = 0.85
                    }
                }
            }
        }

        Rectangle{
            id: plotIconGroup
            width:parent.width * 0.272
            height: parent.height * 0.625
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.025
            anchors.verticalCenter: parent.verticalCenter

            Component {
                id: switchButtonIcon

                Rectangle{
                    id: clickArea
                    width: 60
                    height: 25
                    radius: 5
                    color: isSelected ? selectColor : disSelectColor
                    border.width: isSelected ? 0 : 1
                    border.color: "#F2F2F2"

                    property string buttonText : 'value'
                    property color selectColor : "#999"
                    property color disSelectColor: "#FFF"
                    property color selectTextColor : "#333"
                    property color disSelectTextColor: "#FFF"
                    property bool isSelected: false

                    Label{
                        id: clickAreaText
                        text:buttonText
                        font.family: "微软雅黑"
                        font.pixelSize: 12
                        font.letterSpacing: 2
                        color: isSelected ? disSelectTextColor : selectTextColor
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton
                        onClicked:{
                            if (isSelected){
                                clickArea.color =  disSelectColor
                                clickArea.border.width = 1
                                clickAreaText.color = selectTextColor
                                isSelected = false
                            }
                            else{
                                clickArea.color =  selectColor
                                clickArea.border.width = 0
                                clickAreaText.color = disSelectTextColor
                                isSelected = true
                            }
                        }
                    }
                }
            }

            Row{
                spacing: 10
                Loader {
                    id: dataPlotMode
                    sourceComponent: switchButtonIcon
                    onLoaded: {
                        item.buttonText = "数据"
                        item.isSelected = true
                    }
                }

                Loader {
                    id: implancePlotMode
                    sourceComponent: switchButtonIcon
                    onLoaded: {
                        item.buttonText = "阻抗"
                    }
                }

                Loader {
                    id: ratePlotMode
                    sourceComponent: switchButtonIcon
                    onLoaded: {
                        item.buttonText = "频谱"
                    }
                }
            }
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

