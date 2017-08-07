import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Controls.Private 1.0

Item {
    width: rowContent.width
    height: rowContent.height

    property alias comboxModel: rowComobox.model
    property alias labelText: rowLabel.text
    property alias rowWidth: rowContent.width
    property alias rowHeight: rowContent.height
    property real comboBoxWidth: 120
    property real comboBoxHeight: 25
    property color labelTextColor: "#333"

    Rectangle{
        id: rowContent
        anchors.verticalCenter: parent.verticalCenter

        Label{
            id: rowLabel
            font.family:"宋体"
            font.pixelSize:12
            color: labelTextColor
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
        }

        ComboBox{
            id: rowComobox
            width: comboBoxWidth
            height: comboBoxHeight
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: rowLabel.right
            anchors.leftMargin: 10

            style: ComboBoxStyle {
                    id: comboBox
                    background: Rectangle {
                        id: rectCategory
                        border.width: 1
                        border.color: "#E4E4E4"

                        Image {
                            source: "img/arrow-down.png"
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            scale: 0.8
                        }
                    }
                    label: Text {
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        font.family: "宋体"
                        color: labelTextColor
                        text: control.currentText
                    }

                    // drop-down customization here
                    property Component __dropDownStyle: MenuStyle {
                        __maxPopupHeight: 600
                        __menuItemType: "comboboxitem"

                        frame: Rectangle {              // background
                            color: labelTextColor
                            border.color: "#6FF"
                            border.width: 1
                        }

                        itemDelegate.label:             // an item text
                            Text {
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 12
                            font.family: "宋体"
                            color: labelTextColor
                            text: styleData.text
                        }

                        itemDelegate.background: Rectangle {  // selection of an item
                            height: comboBoxHeight
                            color: styleData.selected ? "#EBEBEB" : "#FFFFFF"
                        }

                        __scrollerStyle: ScrollViewStyle { }
                    }

                    property Component __popupStyle: Style {
                        property int __maxPopupHeight: 400
                        property int submenuOverlap: 0

                        property Component frame: Rectangle {
                            width: (parent ? parent.contentWidth : 0)
                            height: (parent ? parent.contentHeight : 0)
                            property real maxHeight: 500
                            property int margin: 1
                        }

                        property Component menuItemPanel: Text {
                            text: "NOT IMPLEMENTED"
                            color: "red"
                            font {
                                pixelSize: 14
                                bold: true
                            }
                        }

                        property Component __scrollerStyle: null
                    }
                }
        }
    }
}
