import QtQuick 2.0
import QtQuick.Window 2.0

Rectangle {
    id: titleArea
    width: parent.width
    height: 40
    anchors.left: parent.left
    anchors.top: parent.top
    color: "#003333"

    property alias iconPath: iconPic.source
    property color sleepColor: "#58F"
    property color activeColor: "#6CF"
    property var target: null

    Component.onCompleted: {
        titleArea.iconPath = "/img/logo.png"
    }

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
                        }
                    }
                }
            }
        }
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
            width: parent.width * 0.1
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: iconPic
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle{
            id: toolIconGroup
            width: parent.width * 0.107
            height: parent.height * 0.425
            anchors.right: titleIconSplitLine.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: parent.width * 0.031
            color: parent.color

            Row{
                anchors.centerIn: parent
                spacing: toolIconGroup.width * 0.2

                Loader {
                    id: bluetoothIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/bluetooth.png"
                        item.imgScale = 0.975
                    }

                    Connections{
                        target: bluetoothIcon.item
                        onIconClicked: {
                            var rootItemComponent = titleArea.target.independentComponent
                            if(rootItemComponent === null){
                                rootItemComponent = Qt.createComponent("DeviceTestWindow.qml");
                            }
                            if(rootItemComponent.status === Component.Ready) {
                                var wx = (titleArea.target.width - 510) / 2
                                var wy = (titleArea.target.height - 315) / 2
                                titleArea.target.independentInstance = rootItemComponent.createObject(titleArea.target, {"target":titleArea.target, "x":wx, "y":wy});
                            }
                        }
                    }
                }

                Loader {
                    id: styleIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/style.png"
                        item.imgScale = 0.975
                    }
                }

                Loader {
                    id: settingIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/setting.png"
                        item.imgScale = 0.975
                    }

                    Connections{
                        target: settingIcon.item
                        onIconClicked: {
                            var rootItemComponent = titleArea.target.floatComponent
                            if(rootItemComponent === null){
                                rootItemComponent = Qt.createComponent("GeneralSettingPanel.qml")
                            }

                            if (titleArea.target.floatInstance === null){
                                if(rootItemComponent.status === Component.Ready) {
                                    var wx = titleArea.target.width*0.99 - 510
                                    var wy = titleArea.height
                                    titleArea.target.floatInstance = rootItemComponent.createObject(titleArea.target, {"parentRef":titleArea.target, "x":wx, "y":wy});
                                }
                            }
                            else{
                                titleArea.target.floatInstance.destroy()
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: titleIconSplitLine
            width: 1
            height: parent.height * 0.3
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: windowControlIconGroup.left
            anchors.rightMargin: parent.width * 0.031
            color: "#868686"
        }

        Rectangle{
            id: windowControlIconGroup
            width: parent.width * 0.088
            height: parent.height * 0.3
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            color: parent.color

            Row{
                anchors.centerIn: parent
                spacing: windowControlIconGroup.width * 0.2

                Loader {
                    id: minIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/min.png"
                    }

                    Connections{
                        target: minIcon.item
                        onIconClicked: {
                            titleArea.target.visibility = Window.Minimized
                        }
                    }
                }

                Loader {
                    id: maxIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/max.png"
                    }

                    Connections{
                        target: maxIcon.item
                        onIconClicked: {
                            if (titleArea.target.visibility === Window.Maximized){
                                titleArea.target.visibility = Window.Windowed
                            }
                            else if (titleArea.target.visibility === Window.Windowed){
                                titleArea.target.visibility = Window.Maximized
                            }
                        }
                    }
                }

                Loader {
                    id: closeIcon
                    sourceComponent: iconItem
                    onLoaded: {
                        item.imgSource = "/img/close.png"
                    }

                    Connections{
                        target: closeIcon.item
                        onIconClicked: {
                            Qt.quit()
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        property bool isPress: false
        property int mousePosPreX
        property int mousePosPreY

        id: mouseArea
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
                target.x += mouseX - mousePosPreX
                target.y += mouseY - mousePosPreY
            }
        }
    }
}


