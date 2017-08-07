import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: normalCheckBox
    width: sourceCheckbox.width
    height: sourceCheckbox.height

    property alias checkboxText: sourceCheckbox.cText
    property color textColor: "#333"
    property real cSpacing: 12
    property bool isChecked: sourceCheckbox.checked
    property alias cExclusiveGroup: sourceCheckbox.exclusiveGroup

    CheckBox{
        id: sourceCheckbox

        property string cText: "value"

        style: CheckBoxStyle{
            spacing: cSpacing
            label:Label{
                color: textColor
                font.family: "宋体"
                font.pixelSize: 12
                text: sourceCheckbox.cText
            }
            indicator: Rectangle {
                            border.color: "#6CF"
                            border.width: 1
                            implicitHeight: 15
                            implicitWidth: 15
                            Rectangle {
                                visible: control.checked
                                color: "#74CFFA"
                                border.color: "#6CF"
                                anchors.margins: 4
                                anchors.fill: parent
                            }
                    }
        }
    }
}

