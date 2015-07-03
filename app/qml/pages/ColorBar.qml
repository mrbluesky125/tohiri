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

    Connections { target: colorMap; onColorStopsChanged: canvas.requestPaint() }
    Connections { target: colorMap; onLevelChanged: canvas.requestPaint() }
    Connections { target: colorMap; onWindowChanged: canvas.requestPaint() }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = canvas.getContext("2d");
            ctx.clearRect(0,0,canvas.width,canvas.height);

            var grd=ctx.createLinearGradient(0,0,canvas.width,0);
            for(var position = 0; position <= 1.0; position += 0.1) {
                grd.addColorStop(position, colorMap.getColor(minValue + ((maxValue-minValue)*position)));
            }

            ctx.fillStyle=grd;
            ctx.fillRect(0,0,canvas.width,canvas.height);
        }
    }
}
