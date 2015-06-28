import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

import harbour.tohiri.TohIR 1.0

Item {
    id: rootColorBar

    width: 100
    height: 62

    property ColorMap colorMap
    property real minValue: -20
    property real maxValue: 100

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(width, 0)
        gradient: Gradient {
            GradientStop { position: 0.0; color: colorMap.getColor(minValue + (maxValue-minValue)*0.0) }
            GradientStop { position: 0.1; color: colorMap.getColor(minValue + (maxValue-minValue)*0.1) }
            GradientStop { position: 0.2; color: colorMap.getColor(minValue + (maxValue-minValue)*0.2) }
            GradientStop { position: 0.3; color: colorMap.getColor(minValue + (maxValue-minValue)*0.3) }
            GradientStop { position: 0.4; color: colorMap.getColor(minValue + (maxValue-minValue)*0.4) }
            GradientStop { position: 0.5; color: colorMap.getColor(minValue + (maxValue-minValue)*0.5) }
            GradientStop { position: 0.6; color: colorMap.getColor(minValue + (maxValue-minValue)*0.6) }
            GradientStop { position: 0.7; color: colorMap.getColor(minValue + (maxValue-minValue)*0.7) }
            GradientStop { position: 0.8; color: colorMap.getColor(minValue + (maxValue-minValue)*0.8) }
            GradientStop { position: 0.9; color: colorMap.getColor(minValue + (maxValue-minValue)*0.9) }
            GradientStop { position: 1.0; color: colorMap.getColor(minValue + (maxValue-minValue)*1.0) }
        }
    }
}
