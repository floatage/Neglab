import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: textRow
    width: textRowCotent.width
    height: textRowCotent.height

    property alias rowText: rowLabel.text
    property alias rSpacing: textRowCotent.spacing
    property alias tPlaceholderText: rowTextField.placeholderText
    property color textColor: "#333"
    property real tWidth: 120
    property real tHeight: 25
    property bool hasButton: false
    property string bButtonText: "default"
    property string value: rowTextField.text

    Row{
        id: textRowCotent
        spacing: 10

        property var rowButton: null

        Label{
            id: rowLabel
            font.family:"宋体"
            font.pixelSize:12
            color: textColor
            anchors.verticalCenter: parent.verticalCenter

            Component.onCompleted: {
                if (hasButton){
                    var obj = Qt.createComponent("NormalButton.qml");
                    textRowCotent.rowButton = obj.createObject(textRowCotent, {buttonText:bButtonText,hasBorder: false,fillHeight: 0,fillWidth:0})
                    textRowCotent.rowButton.anchors.verticalCenter =  rowLabel.verticalCenter
                    textRowCotent.spacing = 7
                }
            }
        }

        TextField{
            id: rowTextField
            anchors.verticalCenter: rowLabel.verticalCenter
            style:TextFieldStyle{
                font: rowLabel.font
                textColor: textColor
                background: Rectangle {
                      implicitWidth: tWidth - (hasButton ? textRowCotent.rowButton.width : 0)
                      implicitHeight: tHeight
                      border.color: control.activeFocus ? "#6CF" : "#E4E4E4"
                      border.width: 1
                }
            }
        }
    }
}

