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
            captureMode: Camera.CaptureStillImage
            focus.focusMode: Camera.FocusContinuous
            onError: {
                console.error("error: " + camera.errorString);
            }
        }

        PageHeader
        {
            anchors.top: parent.top
            id: cameraHeader
            title: "TOH Infrared Imager"
        }



        VideoOutput
        {
            id: videoPreview
            source: camera
            width: 480
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -60
        }

        Grid {
            id: irView

            anchors.fill: videoPreview
            height: 640
            width: 480
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
                    Behavior on color { ColorAnimation { duration: tohir.updateRate*0.75 } }
                }
            }

            layer.enabled: true
            layer.effect: GaussianBlur {
                cached: true
                source: irView
                radius: Math.min(irView.tileHeight, irView.tileWidth)*tohir.granularity
                samples: 32
            }
        }

        Grid {
            id: irOverlay

            anchors.fill: videoPreview
            height: 640
            width: 480
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

        Rectangle
        {
            id: mamBackground
            z: 2
            color: Theme.secondaryHighlightColor
            width: 480
            height: mamLabels.height + mamValues.height + 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: irView.bottom

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
                }
                Label
                {
                    width: page.width/3
                    text: "avg"
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
                }
                Label
                {
                    width: page.width/3
                    text: "max"
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
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
                }
                Label
                {
                    width: page.width/3
                    text: tohir.avgTemp.toFixed(1) + " ºC"
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
                }
                Label
                {
                    width: page.width/3
                    text: tohir.maxTemp.toFixed(1) + " ºC"
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
                }
            }
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

        Row
        {
            id: thermistorDateRow
            spacing: Theme.paddingLarge
            anchors.top: mamBackground.bottom
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
    }

    TohIR {
        id: tohir

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
        onTriggered:
        {
            tohir.readThermistor()
            currentDateTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss - dd.MM.yyyy")
        }
    }

}


