import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: plotArea

    property int channelNum: 8
    property real channelTitleHeight: height / channelNum
    property var channelStateList: []
    property var channelBaselineList: []
    property int channelBaselineOffset: 0

    property var prePoints: []
    property var curPoints: []

    //每次绘图x坐标移动的距离
    property real xOffset: 1
    //屏幕清除的宽度
    property real clearWidth: 50
    //最大绘图次数，超出则刷新
    property int flushTimes: 8
    //绘图间隔，隔几次画一次
    property int plotInterval: 1
    //数据扩大倍数
    property real plotDataMultiplier: 100
    property real plotDataOffset: 0

    Component{
        id: channelTitleLabel

        Rectangle{
            width: channelNameColumnArea.width
            height: channelTitleHeight

            property int channelIndex: -1

            Label{
                anchors.centerIn: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: 'T' + channelIndex
                color: "#666"
                font.family: "微软雅黑"
                font.pixelSize: 10
            }
        }
    }

    Component{
        id: channelCheckbox

        Rectangle{
            property int channelIndex: -1

            width: channelSelectColumnArea.width
            height: channelTitleHeight

            CheckBox{
                checked: true;
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right

                onCheckedChanged: {
                    channelStateList[channelIndex] = checked;
                }
            }
        }
    }

    function clacChannelBaseline(){
        var baselineY = channelTitleHeight / 2
        channelBaselineList = []
        for(var begin = 0; begin < channelNum; ++begin){
            if (begin === 0){
                channelBaselineList[begin] = baselineY + channelBaselineOffset
            }else{
                channelBaselineList[begin] = channelBaselineList[begin-1] + channelTitleHeight
            }
        }
    }

    Component.onCompleted: {
        for (var begin = 0; begin < channelNum; ++begin){
            var createTitleString = "import QtQuick 2.0; Loader {id: channelTitle" + begin + "; sourceComponent: channelTitleLabel; onLoaded:{item.channelIndex="+ begin +";}}";
            Qt.createQmlObject(createTitleString ,channelNameColumn, "");
            var createCheckboxString = "import QtQuick 2.0; Loader {id: channelCheckbox" + begin + "; sourceComponent: channelCheckbox; onLoaded:{item.channelIndex="+ begin +";}}";
            Qt.createQmlObject(createCheckboxString ,channelSelectColumn, "");
            channelStateList.push(true);
            clacChannelBaseline()
        }
    }

    Connections{
        target: parent
        onWidthChanged:{
            width = parent.width * 0.96
        }

        onHeightChanged:{
            height = (parent.height - 40) * 0.96
            clacChannelBaseline()
        }

        onChannelDataUpdate: {
            prePoints = curPoints
            curPoints = newData
            channelPlotCanvas.paint(null)
        }
    }

    Rectangle{
        id: graphShowArea
        width: parent.width
        height: parent.height

        Rectangle{
            id: channelNameColumnArea
            width: parent.width * 0.025
            height: parent.height
            anchors.left: parent.left

            Column{
                id: channelNameColumn
                width: parent.width
                height: parent.height
            }
        }

        Rectangle{
            id: channelPlotArea
            width: parent.width * 0.95
            height: parent.height
            anchors.left: channelNameColumnArea.right

            Canvas{
                id: channelPlotCanvas
                width: parent.width
                height: parent.height
                renderTarget: Canvas.FramebufferObject
                renderStrategy: Canvas.Threaded
                property real maxX: width
                property real curX: 0
                property int curPlotTimes: 0
                property int curPlotIntervalCounter: 0

                function clearCanvas(ctx, x) {
                    var clearStartX = x + 1
                    clearStartX = clearStartX >= maxX || x <= 0 ? 0 : clearStartX
                    ctx.fillStyle="#FFF"
                    ctx.beginPath()
                    ctx.fillRect(clearStartX, 0, clearWidth, channelPlotCanvas.height)
                    ctx.closePath()
                }

                onPaint: {
                    if (curPoints === [] || prePoints === []) return

                    curPlotIntervalCounter = ++curPlotIntervalCounter % plotInterval
                    if (curPlotIntervalCounter !== 0) return

                    var context = getContext('2d')
                    if (curX <= 0){
                        clearCanvas(context, curX)
                        curX += xOffset
                        return
                    }

                    curX = curX >= maxX ? 0 : curX

                    //绘制控制信号
                    if (curPoints[0] !== 0){
                        context.strokeStyle = "#0F0"
                        context.moveTo(curX-xOffset, 0)
                        context.lineTo(curX-xOffset, channelPlotCanvas.height)
                        context.stroke()
                    }

                    context.strokeStyle = "#000"
                    //绘图第一个数据为控制信号，则跳过，由于控制信号影响导致下标不匹配，则基线数组和控制数组下标均需减一
                    for (var begin = 1, end = prePoints.length; begin < end; ++begin){
                        if (begin >= curPoints.length) break

                        if (channelStateList[begin-1]){
                            context.moveTo(curX-xOffset, (prePoints[begin] * plotDataMultiplier) + channelBaselineList[begin-1] + plotDataOffset)
                            context.lineTo(curX, (curPoints[begin] * plotDataMultiplier) + channelBaselineList[begin-1] + plotDataOffset)
                        }
                    }

                    context.stroke()
                    clearCanvas(context, curX)
                    curX += xOffset

                    //定次刷新
                    ++curPlotTimes
                    if (curPlotTimes >= flushTimes ) {
                        curPlotTimes = 0
                        markDirty()
                    }
                }
            }
        }

        Rectangle{
            id: channelSelectColumnArea
            width: parent.width * 0.025
            height: parent.height
            anchors.left: channelPlotArea.right

            Column{
                id: channelSelectColumn
                width: parent.width
                height: parent.height
            }
        }
    }
}


//Switch{
//    id: channelSwitch
//    anchors.left: channelNum.right
//    anchors.leftMargin: 5
//    anchors.verticalCenter: channelNum.verticalCenter
//    checked: true
//    style: SwitchStyle {
//            groove: Rectangle {
//                    implicitWidth: 25
//                    implicitHeight: 14
//                    radius: 9
//                    color: "#EAEAEA"
//            }

//            handle: Rectangle{
//                implicitWidth:14
//                implicitHeight: 12
//                radius: 6
//                color: control.checked ? "#6CF" : "#FFF"
//                border.color: control.checked ? "#EAEAEA" : "#6FF"
//                border.width: 1
//            }
//        }
//}
