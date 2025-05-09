import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 800
    height: 600
    visible: true
    title: "Kamera z OpenCV + QML"

    Image {
        id: imageView
        width: 640
        height: 480
        cache: false
        anchors.centerIn: parent
        source: ""
        fillMode: Image.PreserveAspectFit
    }

    // Zmieniamy - połączmy sygnał z C++ z aktualizacją obrazu w QML
    Connections {
        target: processor
        function onImageUpdated() {
            imageView.source = "" // Wymuszenie odświeżenia obrazu
            imageView.source = "image://opencv/live" // Wymuszenie odświeżenia obrazu
        }
    }

    Button {
        text: "Start Kamera"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        onClicked: {
            processor.startCamera()
        }
    }

    Button {
        text: "Stop Kamera"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        onClicked: {
            processor.stopCamera()
        }
    }
}
