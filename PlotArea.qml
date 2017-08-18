import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: plotArea

    property int channelNum: 8
    property real channelTitleHeight: height / channelNum
    property var channelStateList: []
    property var channelBaselineList: []

    Component{
        id: channelTitleLabel

        Rectangle{
            width: channelNameColumnArea.width
            height: channelTitleHeight
//            border.color: "#000"

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
//            border.color: "#000"

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
                channelBaselineList[begin] = baselineY - 5
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
            height =  (parent.height - 40) * 0.96
            clacChannelBaseline()
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

                property var prePoints: [10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10]
                property var curPoints: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

                Timer {
                  interval: 100; running: true; repeat: true
                  onTriggered: channelPlotCanvas.paint(null)
                }

                function makeWhite(ctx, x, y, w, h) {
//                    ctx.strokeStyle = "#FFF"
//                    ctx.beginPath();
//                    ctx.rect(x, y, w, h);
//                    ctx.closePath();
//                    ctx.stroke()
                    ctx.fillStyle="#FFF";
                    ctx.beginPath();
                    ctx.fillRect(x,y,w,h);
                    ctx.closePath();
                }

                onPaint: {
                    console.log('dwa')
                    if (curX >= maxX) curX = 0
                    var context = getContext('2d')

                    for (var begin = 0; begin < channelNum;++begin){
                        if (channelStateList[begin]){
                            context.moveTo(curX-2, prePoints[begin] + channelBaselineList[begin])
                            context.lineTo(curX, curPoints[begin] + channelBaselineList[begin])
                        }
                    }
                    context.stroke()

                    var clearStartX = curX + 1
                    if (clearStartX >= maxX) clearStartX = 0
                    makeWhite(context, clearStartX, 0, 50, channelPlotCanvas.height)

                    curX+=10
                    markDirty()
                    var tmp = prePoints
                    prePoints = curPoints
                    curPoints = tmp
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
