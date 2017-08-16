import QtQuick 2.0
import QtQuick.Window 2.2

Window {
    id: mainWindow
    width: 1000
    height: 560
    flags: Qt.FramelessWindowHint | Qt.Window
    visible: true

    property color textColor: "#333"
    property string textFontFamily: "宋体"
    property int textFontPixelSize: 12
    property Component independentComponent: null
    property var independentInstance: null
    property Component floatComponent: null
    property var floatInstance: null

    function independentInstanceDestory(){
        independentInstance.destroy()
        independentInstance = null
    }

    EeglabTitle{
        id: mainTitle
        target: mainWindow
    }

    PlotControlPanel{
        id: plotControlPanel
        width: parent.width * 0.266
        height: parent.height - mainTitle.height
        anchors.top: mainTitle.bottom
    }

    PlotPanel{
        id: plotPanel
        parentRef: mainWindow
        width: parent.width - plotControlPanel.width
        height: parent.height - mainTitle.height
        anchors.left: plotControlPanel.right
        anchors.top: mainTitle.bottom
    }
}

