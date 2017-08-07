import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: normalRadioButton
    width: sourceRadioButton.width
    height: sourceRadioButton.height

    property alias radioButtonText: sourceRadioButton.cText
    property color textColor: "#949494"
    property real cSpacing: 5
    property bool isChecked: sourceRadioButton.checked
    property alias rExclusiveGroup: sourceRadioButton.exclusiveGroup

    RadioButton{
        id: sourceRadioButton

        property string cText: "value"

        style: RadioButtonStyle {
            spacing: cSpacing
            label:Label{
                color: textColor
                font.family: "宋体"
                font.pixelSize: 12
                text: sourceRadioButton.cText
            }
            indicator: Rectangle {
                    implicitWidth: 14
                    implicitHeight: 14
                    radius: 7
                    border.color: "#6CF"
                    border.width: 1
                    Rectangle {
                        anchors.fill: parent
                        visible: control.checked
                        color: "#69F"
                        radius: 7
                        anchors.margins: 4.5
                    }
            }
        }
    }
}
