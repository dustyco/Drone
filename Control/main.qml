
import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import Drone 1.0

Window {
    visible: true
    width: 640
    height: 480

    property real em: (width+height)/50

    DroneControl {
        id: control

		NumberAnimation on frameTrigger {
			from: 0
			to: 350
			loops: Animation.Infinite
			duration: 2000
		}
    }


    Image {
        source: "http://i.ytimg.com/vi/H5Po0gwuFqY/maxresdefault.jpg"
        anchors.fill: parent
    }

    Rectangle {
        width: row.width + em
        height: row.height
        anchors.horizontalCenter: parent.horizontalCenter
        y: border.width*-1

        color: "#80FFFFFF"
        border.color: "black"
        border.width: em*0.1

        RowLayout {
            id: row

            Text {
                x: 50
                text: control.networkStatus
                font.family: "monospace"
                font.pixelSize: em*1
                color: "black"
            }

            Text {
                x: 50
                text: "Traffic: " + control.traffic + " Bytes"
                font.family: "monospace"
                font.pixelSize: em*1
                color: "black"
            }
        }

    }

}
