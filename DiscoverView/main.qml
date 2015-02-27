
import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import Drone 1.0

// 0.33333333333333333333 KB/s
// 3 s/KB
// min/GB

Window {
    visible: true
    width: 640
    height: 480

		property real em: (width+height)/100

    DroneDiscoverView {
        id: discoverView
    }

    Rectangle {
        width: row.width + em
        height: row.height
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.centerIn: parent
        y: border.width*-1

        color: "#80FFFFFF"
        border.color: "black"
				border.width: em*0.1

        RowLayout {
            id: row

            Text {
                x: 50
                text: discoverView.text
                font.family: "monospace"
                font.pixelSize: em*1
                color: "black"
            }
        }

    }

}
