import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: optionalTextRow
    width: rowContent.width
    height: rowContent.height

    property alias rowValue: rowTextField.text
    property alias rowIsChecked: rowContentCheckbox.isChecked
    property alias checkboxText: rowContentCheckbox.checkboxText
    property alias placeholderText: rowTextField.placeholderText
    property alias rowWidth: rowContent.width
    property alias rowHeight: rowContent.height
    property alias textField: rowTextField
    property real textWidth: 120
    property color textColor: "#333"
    property string unitText: ''
    property var inputValidator: DoubleValidator{decimals:2}
    property real controlSpacing: 10

    Row{
        id: rowContent
        spacing: controlSpacing

        NormalCheckbox{
            id: rowContentCheckbox
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField{
            id: rowTextField
            font.family: "宋体"
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
            validator: optionalTextRow.inputValidator

            style:TextFieldStyle{
                font: rowTextField.font
                textColor: optionalTextRow.textColor
                background: Rectangle {
                      implicitWidth: textWidth
                      implicitHeight: 25
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
            text: optionalTextRow.unitText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

