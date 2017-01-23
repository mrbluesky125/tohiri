import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    width: 100
    height: 62
    clip: true

    property real minValue: 0
    property real maxValue: 1
    property real level: 0.5
    property real window: 1

    property real lowerWindowValue: level-(window/2)
    property real upperWindowValue: level+(window/2)

    function valueToPixel(value) {
        return ((value-minValue)/(maxValue-minValue)) * width;
    }

    function pixelToValue(pixel) {
        return (pixel/width)*(maxValue-minValue)+minValue;
    }

    Rectangle {
        id: lowerWindowBar
        height: parent.height + border.width*2
        width: valueToPixel(root.window)
        anchors.verticalCenter: parent.verticalCenter
        x: valueToPixel(root.level)-(width/2)
        color: "#1F7F7F7F"
        border.width: 2
        border.color: "white"

        Rectangle {
            anchors.verticalCenter: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            rotation: 45
            height: 20
            width: 20
        }

        Rectangle {
            anchors.verticalCenter: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            rotation: 45
            height: 20
            width: 20
        }

        Label
        {
            anchors.centerIn: parent
            text: level.toFixed(1) + " ºC"
            color: Theme.primaryColor
            font.pixelSize: Theme.fontSizeSmall
            horizontalAlignment: Text.AlignHCenter
            style: Text.Outline
        }

        MouseArea {
            anchors.fill: parent
            drag.target: lowerWindowBar
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: root.width
            onPositionChanged: {
                root.level = pixelToValue(lowerWindowBar.x+(lowerWindowBar.width/2));
            }
        }
    }
}
