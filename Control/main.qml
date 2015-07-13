
import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import Drone 1.0

// 0.33333333333333333333 KB/s
// 3 s/KB
// min/GB

Window {
	id: window
    visible: true
		width: 800
		height: 600

    property real em: (width+height)/75

    DroneControl {
			id: control
			onLogSignal: {
				if (level=="Debug") return
				logText.text += tag + " " + level + "  |  " + text + "\n"
			}

			NumberAnimation on frameTrigger {
				from: 0
				to: 350
				loops: Animation.Infinite
				duration: 2000
			}
    }


    Image {
				//source: "http://i.ytimg.com/vi/H5Po0gwuFqY/maxresdefault.jpg"
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
								text: control.info
								font.family: "sans"
								font.pixelSize: em*1
								color: "black"
						}
							 /*
						Text {
								x: 50
								text: "Traffic: " + Math.round(control.traffic/1000) + " KB"
								font.family: "monospace"
								font.pixelSize: em*1
								color: "black"
						}
						*/
				}

		}
		Rectangle {
			width: logText.width + em
			height: logText.height
			anchors.horizontalCenter: parent.horizontalCenter
			y: border.width*-1 + window.height - height

			color: "#80FFFFFF"
			border.color: "black"
			border.width: em*0.1

			Text {
					x: 50
					id: logText
					font.family: "sans"
					font.pixelSize: em*1
					color: "black"
			}

	}

}
