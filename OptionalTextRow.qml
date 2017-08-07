import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    width: rowContent.width
    height: rowContent.height

    property alias checkboxText: rowContentCheckbox.checkboxText
    property alias placeholderText: rowTextField.placeholderText
    property alias rowWidth: rowContent.width
    property alias rowHeight: rowContent.height
    property real textWidth: 120

    Rectangle{
        id: rowContent

        NormalCheckbox{
            id: rowContentCheckbox
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        TextField{
            id: rowTextField
            font.family: "宋体"
            font.pixelSize: 12
            anchors.left: rowContentCheckbox.right
            anchors.leftMargin: parent.width * 0.06
            anchors.verticalCenter: parent.verticalCenter

            style:TextFieldStyle{
                font: rowTextField.font
                textColor: "#333"
                background: Rectangle {
                      implicitWidth: textWidth
                      implicitHeight: 25
                      border.color: control.activeFocus ? "#6CF" : "#E4E4E4"
                      border.width: 1
                }
            }
        }
    }
}

