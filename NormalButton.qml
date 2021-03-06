import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: normalButton
    width: sourceButton.width
    height: sourceButton.height

    property alias buttonText: sourceButton.bText
    property real fillWidth: 30
    property real fillHeight: 18
    property real buttonTextSize: 12
    property color buttonTextColor: "#333"
    property bool hasBorder: true
    property real buttonRadius: 0
    property alias button: sourceButton

    Button{
        id: sourceButton
        property string bText: "value"

        style: ButtonStyle {
                label: Label{
                    color: control.hovered ? "#FFF": buttonTextColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.family: "宋体"
                    font.pixelSize: buttonTextSize
                    text: sourceButton.bText
                }

                background: Rectangle {
                    Label{
                        id: textInfor
                        visible: false
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.family: "宋体"
                        font.pixelSize: buttonTextSize
                        text: sourceButton.bText
                    }

                    radius: normalButton.buttonRadius
                    implicitWidth: textInfor.contentWidth + fillWidth
                    implicitHeight: textInfor.contentHeight + fillHeight
                    border.color: control.pressed ? "#6CF" : "#E4E4E4"
                    border.width: hasBorder ? (control.pressed ? 2 : 1) : 0
                    color: control.hovered ? buttonTextColor : "#FFF"
                }
        }
    }
}

