import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.tohiri.TohIR 1.0
import QtMultimedia 5.0
import QtGraphicalEffects 1.0

import "."

Page
{
    id: page

    property bool appActive: applicationActive
    property bool pageActive: page.status === PageStatus.Active

    onAppActiveChanged:
    {
        if (!appActive)
        {
            console.log("stopping camera")
            camera.stop()
        }
        else if (camera.cameraStatus === 4) // LoadedStatus
        {
            console.log("starting camera")
            camera.start()
        }
    }

    Messagebox
    {
        id: messagebox
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: contentColumn.height

        PullDownMenu {
            id: pdm
            visible: !saveTimer.running

            MenuItem {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("aboutPage.qml"),
                                          { "version": tohir.version } )
            }
            MenuItem {
                text: "Settings"
                onClicked:
                {
                    var dialog = pageStack.push(Qt.resolvedUrl("SettingsDialog.qml"),
                                                { "gradientOpacity": tohir.gradientOpacity,
                                                    "updateRate": tohir.updateRate,
                                                    "granularity": tohir.granularity,
                                                    "contrast": tohir.contrast } )
                    dialog.accepted.connect( function()
                    {
                        tohir.gradientOpacity = dialog.gradientOpacity
                        tohir.updateRate = dialog.updateRate
                        tohir.granularity = dialog.granularity
                        tohir.contrast = dialog.contrast
                        tohir.saveSettings()
                    } )
                }

            }
            MenuItem
            {
                text: "Save image"
                onClicked: {
                    saveTimer.start()
                }
            }
        }

        Camera
        {
            id: camera
            flash.mode: Camera.FlashOff
            captureMode: Camera.CaptureViewfinder
            focus.focusMode: Camera.FocusContinuous
            exposure.exposureMode: Camera.ExposurePortrait
            onError: {
                console.error("error: " + camera.errorString);
            }
        }

        Column {
            id: contentColumn
            spacing: 18
            width: parent.width

            PageHeader
            {
                id: cameraHeader
                title: "TOH Infrared Imager"
            }

            VideoOutput
            {
                id: videoPreview
                anchors.horizontalCenter: parent.horizontalCenter
                source: camera
                height: width*(4./3.)
                width: 480

                //Used for colors and shader effects
                Grid {
                    id: irView

                    anchors.centerIn: videoPreview
                    height: videoPreview.height
                    width: videoPreview.width
                    columns: 8
                    rows: 8
                    opacity: tohir.gradientOpacity

                    property int tileHeight: irView.height/irView.rows
                    property int tileWidth: irView.width/irView.columns

                    Repeater {
                        model: tohir
                        delegate: Rectangle {
                            height: irView.tileHeight
                            width: irView.tileWidth
                            color: colorRole

                            //smooth animation between colors - possibly causing higher cpu load
                            Behavior on color { ColorAnimation { duration: tohir.updateRate } }
                        }
                    }

                    layer.enabled: true
                    layer.effect: GaussianBlur {
                        source: irView
                        radius: Math.min(irView.tileHeight, irView.tileWidth)*tohir.granularity
                        samples: 16
                    }
                }

                //Used for additional information - no shader effects
                Grid {
                    id: irOverlay

                    anchors.fill: videoPreview
                    columns: 8
                    rows: 8
                    opacity: tohir.gradientOpacity

                    property int tileHeight: irView.height/irView.rows
                    property int tileWidth: irView.width/irView.columns
                    property int selectedTile: -1

                    Repeater {
                        model: tohir
                        delegate: Item {
                            height: irView.tileHeight
                            width: irView.tileWidth

                            Rectangle {
                                id: hotSpotMarker
                                anchors.centerIn: parent
                                height: Math.min(parent.height/6, parent.width/6)
                                width: Math.min(parent.height/6, parent.width/6)
                                rotation: 45
                                visible: hotspotRole
                            }

                            Label {
                                anchors.centerIn: parent
                                color: Theme.secondaryColor
                                font.pixelSize: Theme.fontSizeSmall
                                horizontalAlignment: Text.AlignHCenter
                                visible: irOverlay.selectedTile === index
                                text: temperatureRole.toFixed(1) + " ºC"
                                style: Text.Outline
                                styleColor: "black"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: irOverlay.selectedTile = index
                            }
                        }
                    }
                }
            }

            ColorBar
            {
                id: mamBackground
                width: 480
                height: mamLabels.height + mamValues.height + 10
                anchors.horizontalCenter: parent.horizontalCenter

                colorMap: tohir.colorMap
                minValue: tohir.minTemp
                maxValue: tohir.maxTemp

                Connections { target: tohir; onMinTempChanged: mamBackground.requestUpdate() }
                Connections { target: tohir; onMaxTempChanged: mamBackground.requestUpdate() }

                Row
                {
                    id: mamLabels
                    z: 3
                    x: Theme.paddingLarge
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: mamBackground.top

                    Label
                    {
                        width: page.width/3
                        text: "min"
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                    Label
                    {
                        width: page.width/3
                        text: "avg"
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                    Label
                    {
                        width: page.width/3
                        text: "max"
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                }
                Row
                {
                    id: mamValues
                    z: 3
                    anchors.top: mamLabels.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    Label
                    {
                        width: page.width/3
                        text: tohir.minTemp.toFixed(1) + " ºC"
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                    Label
                    {
                        width: page.width/3
                        text: tohir.avgTemp.toFixed(1) + " ºC"
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                    Label
                    {
                        width: page.width/3
                        text: tohir.maxTemp.toFixed(1) + " ºC"
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                        style: Text.Outline
                    }
                }
            }

//            ColorBar {
//                id: levelWindowBackground
//                width: 480
//                height: mamBackground.height
//                anchors.horizontalCenter: parent.horizontalCenter

//                colorMap: tohir.colorMap //ColorMap { window: 120; level: 40 }
//                minValue: -20
//                maxValue: 100

//                LevelWindowControl
//                {
//                    id: levelWindowControl
//                    anchors.fill: parent
//                    minValue: -20.0
//                    maxValue: 100.0

//                    level: 30
//                    window: 30
//                }
//            }

            Row
            {
                id: thermistorDateRow
                spacing: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter

                Label
                {
                    id: thermistorTemperature
                    text: (tohir.thermistor < -200) ? "---" : tohir.thermistor.toFixed(1) + " ºC"
                }
                Label
                {
                    id: currentDateTime
                    text: "---"
                }
            }

            Slider
            {
                id: levelSlider
                width: 480
                label: "Level"
                anchors.horizontalCenter: parent.Center
                minimumValue: -20.0
                maximumValue: 100.0
                value: 30
                stepSize: 1.0
                valueText: value.toFixed(0)
            }

            Slider
            {
                id: windowSlider
                width: 480
                label: "Window"
                anchors.horizontalCenter: parent.Center
                minimumValue: 10.0
                maximumValue: 120.0
                value: 30
                stepSize: 1.0
                valueText: value.toFixed(0)
            }
        }
    }

    TohIR {
        id: tohir
//        colorMap.window: levelWindowControl.window
//        colorMap.level: levelWindowControl.level
        colorMap.window: windowSlider.value
        colorMap.level: levelSlider.value

        Behavior on colorMap.level { NumberAnimation { duration: 1000 } }
    }

    Timer
    {
        id: saveTimer
        interval: 500
        repeat: false
        running: false
        onTriggered: {
            messagebox.showMessage("Image saved: " + tohir.saveScreenCapture(), 4000)
        }
    }

    Timer {
        id: tohirTimer
        interval: tohir.updateRate
        repeat: true
        running: appActive && pageActive && (!pdm.active) && (!saveTimer.running)
        triggeredOnStart: true
        onTriggered: {
            tohir.startScan()
        }
    }

    Timer {
        id: thermistorReader
        interval: 1000
        repeat: true
        running: appActive && pageActive
        onTriggered: {
            tohir.readThermistor()
            currentDateTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss - dd.MM.yyyy")
        }
    }

}


