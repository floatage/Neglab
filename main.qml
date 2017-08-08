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

    PlotControlPanel{
        id: plotControlPanel
        width: parent.width * 0.266
        height: parent.height - mainTitle.height
        anchors.top: mainTitle.bottom
    }

    PlotPanel{
        id: plotPanel
        width: parent.width - plotControlPanel.width
        height: parent.height - mainTitle.height
        anchors.left: plotControlPanel.right
        anchors.top: mainTitle.bottom
    }

    DeviceTestWindow{
        id:daw
        anchors.bottom: mainWindow.bottom
    }
}

