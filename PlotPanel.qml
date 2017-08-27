import QtQuick 2.0
import QtQuick.Controls 1.3

//多次点击开始图标会导致内存一直升高的情况

Rectangle{
    id: plotPanel

    property var parentRef: null
    property Component floatComponent: null
    property var floatInstance: null
    property Component plotAreaComponent: null
    property var plotAreaInstance: null

    //0: ready  1: ploting  2: pause  3: finished
    property int plotStatus: 0
    //0: closed  1: search  2: pause 3:data
    property int deviceStatus: 0
    property var gatherInfor: null
    property int channelNum: 16
    property var plotBuffer: []

    signal channelDataUpdate(var newData)

    function gatherMainProcess(){
        if (plotPanel.plotStatus === 0){
            if (plotPanel.deviceStatus === 0){
                DeviceTestManager.openSerialPort(plotPanel.gatherInfor.port)
                DeviceTestManager.sendDataToPort("AT:GS")
                plotPanel.deviceStatus = 1
            }
            else if(plotPanel.deviceStatus === 3){
                plotPanel.plotStatus = 1
                deviceStartIcon.item.imgSource = "/img/pause.png"
                DeviceTestManager.startDataTransfer()
            }
        }
        else if(plotPanel.plotStatus === 1){
            plotPanel.plotStatus = 2
            deviceStartIcon.item.imgSource = "/img/start.png"
            DeviceTestManager.pauseDataTransfer()
            plotPanel.deviceStatus = 2
        }
        else if(plotPanel.plotStatus === 2){
            plotPanel.plotStatus = 1
            deviceStartIcon.item.imgSource = "/img/pause.png"
            plotPanel.deviceStatus = 3
            DeviceTestManager.startDataTransfer()
        }
    }

    Connections{
        target: DeviceTestManager
        onDeviceReadyRead: {
            if (plotPanel.deviceStatus === 1){
                  //此处为需要获取编号才能开始获取数据的蓝牙设备的处理代码
//                var deviceNumStr = readData.match("NAME-#[0-9]+-")
//                if (deviceNumStr !== null)
//                {
//                    deviceNumStr = '' + deviceNumStr
//                    DeviceTestManager.sendDataToPort("AT:GI-#" + deviceNumStr.match("[0-9]+"))
//                    plotPanel.deviceStatus = 3
//                    gatherMainProcess()
//                }
                plotPanel.deviceStatus = 3
                gatherMainProcess()
            }
//            else if (plotPanel.deviceStatus === 3){
//            }
        }

        onDeviceJudgeFinished: {
            channelNum = DeviceTestManager.getChannelNum()
            channelNum = channelNum === -1 ? 32 : channelNum
            createPlotArea(channelNum)
        }

        onPlotDataReady: {
//            for (var begin = 0, end = Object.keys(plotData).length; begin < end; ++begin){
//                channelDataUpdate(plotData[""+begin])
//            }
//            for (var begin = 0, end = plotData.length; begin < end; ++begin){
//                channelDataUpdate(plotData[begin])
//            }

            if (plotBuffer.length <= 0)
            {
                plotBuffer = plotData
            }
            else
            {
                //为防止定时器可能产生的线程安全问题，采用push而不用concat
                for (var begin = 0, end = plotData.length; begin < end; ++begin){
                    plotBuffer.push(plotData[begin])
                }
            }

            if (!plotTimer.running) plotTimer.restart()
        }
    }

    function createPlotArea(channelNum){
        var rootItemComponent = plotPanel.plotAreaComponent
        if(rootItemComponent === null){
            rootItemComponent = Qt.createComponent("PlotArea.qml");
        }

        if (plotPanel.plotAreaInstance !== null){
            plotPanel.plotAreaInstance.destroy()
            plotPanel.plotAreaInstance = null
            console.log("rebuild plot area")
        }

        if (plotPanel.plotAreaInstance === null &&　rootItemComponent.status === Component.Ready){
            plotPanel.plotAreaInstance = rootItemComponent.createObject(plotPanel, {'channelNum': channelNum,
                                                                            'width': plotPanel.width * 0.96,
                                                                            'height': (plotPanel.height - plotAreaToolBar.height) * 0.96,
                                                                            'anchors.left': plotPanel.left,
                                                                            'anchors.leftMargin': plotPanel.width * 0.02,
                                                                            'anchors.top': plotAreaToolBar.bottom,
                                                                            'anchors.topMargin': (plotPanel.height - plotAreaToolBar.height) * 0.02
                                                                        });
        }
    }

    Timer{
        id: plotTimer
        interval: 10;
        running: false;
        repeat: true
        onTriggered:{
            if (plotBuffer.length > 0)
                channelDataUpdate(plotBuffer.shift())
            else
                plotTimer.stop()
        }
    }

    Component {
        id: iconItem

        Rectangle{
            id: iconRect
            width: iconLen
            height: iconLen

            property real iconLen : 20
            property string imgSource
            property real imgScale: 1.0

            signal iconClicked()

            Image {
                id:iconPic
                source: imgSource
                scale: imgScale
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                property var mask: null

                onClicked: {
                    iconRect.iconClicked()
                }

                onPressed: {
                    if (mask === null){
                        mask = Qt.createQmlObject(
                                    'import QtGraphicalEffects 1.0;ColorOverlay{anchors.fill:iconPic;source:iconPic;color:"#6DF"}',
                                    iconRect, "")
                    }
                    iconPic.scale *= 0.9
                }

                onReleased: {
                    if (mask !== null){
                        mask.destroy()
                        mask = null
                    }
                    iconPic.scale /= 0.9
                }
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
                spacing: (parent.width - creatIcon.width*3)/2

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
            anchors.leftMargin: parent.width * 0.05
            anchors.verticalCenter: parent.verticalCenter

            Row{
                id: deviceIconGroupRow
                spacing: (parent.width - inforIcon.width*4)/3
                anchors.verticalCenter: parent.verticalCenter

                Loader {
                    id: inforIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/userinfor.png"
                        item.imgScale = 0.85
                    }

                    Connections{
                        target: inforIcon.item
                        onIconClicked: {
                            var rootItemComponent = plotPanel.floatComponent
                            if(rootItemComponent === null){
                                rootItemComponent = Qt.createComponent("GatherInforPanel.qml");
                            }

                            if (plotPanel.floatInstance === null){
                                if(rootItemComponent.status === Component.Ready) {
                                    var wx = deviceIconGroup.x - 245
                                    var wy = plotAreaToolBar.y + plotAreaToolBar.height
                                    plotPanel.floatInstance = rootItemComponent.createObject(plotPanel, {"parentRef":plotPanel, "x":wx, "y":wy});
                                }
                            }
                            else{
                                plotPanel.floatInstance.destroy()
                                plotPanel.floatInstance = null
                            }
                        }
                    }
                }

                Loader {
                    id: deviceStartIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/start.png"
                        item.imgScale = 0.85
                    } 

                    Connections{
                        target: deviceStartIcon.item
                        onIconClicked: {
                            if (plotPanel.gatherInfor === null){
                                inforIcon.item.iconClicked()
                                return
                            }

                            gatherMainProcess()
                        }
                    }
                }

                Loader {
                    id: deviceStopIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/stop.png"
                        item.imgScale = 0.85
                    }

                    Connections{
                        target: deviceStopIcon.item
                        onIconClicked: {
                            plotPanel.plotStatus = 0
                            deviceStartIcon.item.imgSource = "/img/start.png"
                            plotPanel.gatherInfor = null
                            DeviceTestManager.finishDataTransfer()
                            plotBuffer = []
                            plotPanel.deviceStatus = 0
                        }
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
                spacing: (parent.width - dataPlotMode.width*3)/2
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
}

