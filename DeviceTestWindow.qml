import QtQuick 2.0
import QtQuick.Controls 1.3

Rectangle{
    id: deviceTestDialog
    width: 510
    height: 315

    property var target: null

    Component {
        id: iconItem

        Rectangle{
            id: iconRect
            width: iconLen
            height: iconLen
            color: titleArea.color

            property real iconLen : 20
            property string imgSource
            property real imgScale: 1.0

            signal iconClicked()

            Image {
                id:iconPic
                source: imgSource
                scale: imgScale
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left

                property var mask: null

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    onClicked: iconRect.iconClicked()

                    onHoveredChanged: {
                        if (iconPic.mask === null){
                            iconPic.mask = Qt.createQmlObject(
                                        'import QtGraphicalEffects 1.0;ColorOverlay{anchors.fill:iconPic;source:iconPic;color:"#FFF"}',
                                        iconRect, "")
                        }
                        else{
                            iconPic.mask.destroy()
                            iconPic.mask = null
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: deviceTestTitleArea
        width: parent.width
        height: 35
        anchors.left: parent.left
        anchors.top: parent.top
        color: "#003333"

        property alias iconPath: iconPic.source
        property color sleepColor: "#58F"
        property color activeColor: "#6CF"

        Component.onCompleted: {
            deviceTestTitleArea.iconPath = "/img/logo.png"
        }

        Rectangle{
            width: parent.width * 0.97
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: parent.width * 0.015
            anchors.rightMargin: parent.width * 0.015
            color: parent.color

            Rectangle {
                id: iconArea
                width: parent.width * 0.2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    id: iconPic
                    anchors.verticalCenter: parent.verticalCenter
                    scale: 0.875
                }
            }

            Rectangle{
                id: windowControlIconGroup
                width: parent.width * 0.15
                height: parent.height * 0.3
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                color: parent.color

                Loader {
                    id: closeIcon
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/close.png"
                        item.imgScale = 0.9
                    }

                    Connections{
                        target: closeIcon.item
                        onIconClicked: {
                            DeviceTestManager.disconnectPort()
                            deviceTestDialog.target.independentInstanceDestory()
                        }
                    }
                }

//                Row{
//                    anchors.centerIn: parent
//                    spacing: windowControlIconGroup.width * 0.2

//                    Loader {
//                        id: minIcon
//                        sourceComponent: iconItem
//                        onLoaded: {
//                            item.imgSource = "/img/min.png"
//                            item.imgScale = 0.875
//                        }

//                        Connections{
//                            target: minIcon.item
//                            onIconClicked: {
//                                console.log('daw')
//                            }
//                        }
//                    }

//                    Loader {
//                        id: maxIcon
//                        sourceComponent: iconItem
//                        onLoaded: {
//                            item.imgSource = "/img/max.png"
//                            item.imgScale = 0.875
//                        }
//                    }

//                    Loader {
//                        id: closeIcon
//                        sourceComponent: iconItem
//                        onLoaded: {
//                            item.imgSource = "/img/close.png"
//                            item.imgScale = 0.875
//                        }

//                        Connections{
//                            target: closeIcon.item
//                            onIconClicked: {
//                                deviceTestDialog.target.independentInstanceDestory()
//                            }
//                        }
//                    }
//                }
            }
        }

        MouseArea {
            property bool isPress: false
            property int mousePosPreX
            property int mousePosPreY

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            propagateComposedEvents: true

            onPressed: {
                isPress = true
                mousePosPreX = mouseX
                mousePosPreY = mouseY
            }

            onReleased: {
                isPress = false
            }

            onPositionChanged: {
                if (isPress == true){
                    deviceTestDialog.x += mouseX - mousePosPreX
                    deviceTestDialog.y += mouseY - mousePosPreY
                }
            }
        }
    }

    Rectangle{
        width: parent.width
        height: parent.height - deviceTestTitleArea.height
        anchors.top: deviceTestTitleArea.bottom
        border.width: 1
        border.color: "#6FF"

        Rectangle{
            width: parent.width * 0.94
            height: parent.height * 0.9
            anchors.centerIn: parent

            Column {
                width: parent.width
                height: parent.height
                spacing: 12

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.6
                    border.width: 1
                    border.color: "#6FF"

                    ScrollView{
                        width: parent.width * 0.98
                        height: parent.height * 0.96
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: parent.width * 0.005

                        ListView{
                            id: deviceMsgListView
                            spacing: 3
                            delegate: Text {
                                text: showData
                            }
                            model: ListModel{
                            }
                        }
                    }

                    Connections{
                        target: DeviceTestManager
                        onDeviceReadyRead: {
                            deviceMsgListView.model.append({showData:readData})
                            deviceMsgListView.currentIndex = deviceMsgListView.model.count - 1
                            if (deviceMsgListView.model.count === 100){
                                deviceMsgListView.model.clear()
                            }


                            var deviceNumStr = readData.match("NAME-#[0-9]+-")
                            if (deviceNumStr !== null)
                            {
                                deviceNumStr = "" + deviceNumStr
                                deviceListCombox.comboxModel.append({deviceNum:'#' + deviceNumStr.match("[0-9]+")})
                                deviceListCombox.comboxCurrentIndex = 0
                            }
                        }
                    }
                }

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.18

                    TextRow{
                        id: deviceSendMsgTextrow
                        tWidth: parent.width
                        tHeight: parent.height
                        tPlaceholderText: "请输入.."
                    }
                }

                Rectangle{
                    width: parent.width
                    height: parent.height * 0.12

                    Row{
                        width: parent.width * 0.7
                        height: parent.height
                        spacing: 25

                        NormalButton{
                            id: deviceSearchButton
                            buttonText: "搜 索"
                            anchors.bottom: parent.bottom

                            Connections{
                                target: deviceSearchButton.button
                                onClicked: {
                                    DeviceTestManager.searchDevice()
                                }
                            }
                        }

                        Rectangle{
                            id: deviceListComboxRow
                            width: 90
                            height: parent.height
                            anchors.verticalCenter: parent.verticalCenter

                            ComboBoxRow{
                                id: deviceListCombox
                                width: 90
                                height: 30
                                comboBoxWidth: 90
                                comboBoxHeight: 30
                                comboxModel:  ListModel{
                                }
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.leftMargin: -10
                            }
                        }

                        NormalButton{
                            id: deviceConnectButton
                            buttonText: "连 接"
                            anchors.bottom: parent.bottom

                            Connections{
                                target: deviceConnectButton.button
                                onClicked: {
                                    var deviceNum = deviceListCombox.comboxCurrentText
                                    if (deviceListCombox.comboxCurrentText !== ""){
                                        DeviceTestManager.connectDevice(deviceNum.substring(1, deviceNum.length))
                                    }
                                }
                            }
                        }

                        NormalButton{
                            id: deviceDisconnectButton
                            buttonText: "断 开"
                            anchors.bottom: parent.bottom.item
                            Connections{
                                target: deviceDisconnectButton.button
                                onClicked: {
                                    DeviceTestManager.disconnectPort()
                                    deviceListCombox.comboxModel.clear()
                                    deviceListCombox.comboxCurrentIndex = -1
                                }
                            }
                        }
                    }

                    NormalButton{
                        id: msgSendButton
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        buttonText: "发 送"

                        Connections{
                            target: msgSendButton.button
                            onClicked: {
                                if (deviceSendMsgTextrow.rowValue !== ""){
                                    DeviceTestManager.sendDataToPort(deviceSendMsgTextrow.rowValue)
                                    deviceMsgListView.model.append({showData:"SEND: " + deviceSendMsgTextrow.rowValue})
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

