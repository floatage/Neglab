import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    width: rowContent.width
    height: rowContent.height

    property alias labelText: rowLabel.text
    property alias sliderText: rowSliderValue.text
    property alias rowWidth: rowContent.width
    property alias rowHeight: rowContent.height
    property real sliderWidth: 120
    property color labelTextColor: "#333"
    property color sliderTextColor: "#6AF"

    Rectangle{
        id: rowContent

        Label{
            id: rowLabel
            font.family:"宋体"
            font.pixelSize:12
            color: labelTextColor
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
        }

        Slider{
            id: rowSlider
            width: sliderWidth
            value: 0
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: rowLabel.right
            anchors.leftMargin: parent.width * 0.05
            maximumValue: 100
            minimumValue: 0
            stepSize: 1

            onValueChanged: {
                rowSliderValue.text = value + " s"
            }

            style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: rowSlider.width
                        implicitHeight: 5
                        color: "#6FF"
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "#6BF" : "#74CFFA"
                        implicitWidth: 15
                        implicitHeight: 15
                        radius: 7.5
                    }
                }
        }

        Label{
            id: rowSliderValue
            font.family:"宋体"
            font.pixelSize:12
            color: sliderTextColor
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: rowSlider.right
            anchors.leftMargin: parent.width * 0.05
        }
    }
}
