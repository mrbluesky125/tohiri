
import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog
{
    property real gradientOpacity
    property int updateRate

    id: settings

    canAccept: true

    onDone:
    {
        if (result === DialogResult.Accepted)
        {
            gradientOpacity = opacitySlider.value.toPrecision(1)
            updateRate = rateSlider.value
        }
    }

    Column
    {
        id: column

        width: parent.width
        spacing: Theme.paddingLarge
        DialogHeader
        {
            acceptText: "Save"
        }
        Slider
        {
            id: opacitySlider
            width: parent.width-10
            label: "Gradient opacity"
            anchors.horizontalCenter: parent.Center
            minimumValue: 0.0
            maximumValue: 1.0
            value: gradientOpacity.toPrecision(1)
            stepSize: 0.1
            valueText: value.toPrecision(1) * 100 + " %"
        }
        Slider
        {
            id: rateSlider
            width: parent.width-10
            label: "Gradient opacity"
            anchors.horizontalCenter: parent.Center
            minimumValue: 100
            maximumValue: 2000
            value: updateRate
            stepSize: 100
            valueText: value + " ms"
        }
    }
}
