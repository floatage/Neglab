import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: textRow
    width: textRowCotent.width
    height: textRowCotent.height

    property alias rowValue: rowTextField.text
    property alias rowText: rowLabel.text
    property alias rSpacing: textRowCotent.spacing
    property alias tPlaceholderText: rowTextField.placeholderText
    property bool isReadOnly: false
    property color textColor: "#333"
    property real tWidth: 120
    property real tHeight: 25
    property bool hasButton: false
    property string bButtonText: "default"
//    property string value: rowTextField.text
    property string unitText: ''
    property var inputValidator: DoubleValidator{decimals:2}
    property real controlSpacing: 10

    Row{
        id: textRowCotent
        spacing: controlSpacing

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
                    textRowCotent.rowButton = obj.createObject(textRowCotent, {buttonRadius:10,buttonText:bButtonText,hasBorder: false,fillHeight: 0,fillWidth:0})
                    textRowCotent.rowButton.anchors.verticalCenter =  rowLabel.verticalCenter
                    textRowCotent.spacing = 7
                }
            }
        }

        TextField{
            id: rowTextField
            anchors.verticalCenter: rowLabel.verticalCenter
            validator: textRow.inputValidator
            readOnly: textRow.isReadOnly

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

        Label{
            id: rowTextUint
            font.family:"宋体"
            font.pixelSize:12
            color: "#666"
            text: textRow.unitText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

