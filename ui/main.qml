import QtQuick
//import TableModel 0.1
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 1020
    height: 480
    visible: true
    title: qsTr("Proxy Statistics")

    ColumnLayout {
        Button {
            text: qsTr("Update table")
            onClicked: myModel.update()
        }

        TableView {
            width: 1020
            height: 400
            columnSpacing: 1
            rowSpacing: 1
            clip: true
            ScrollIndicator.horizontal: ScrollIndicator { }
            ScrollIndicator.vertical: ScrollIndicator { }
            model: myModel
            delegate: Rectangle {
                implicitWidth: 100
                implicitHeight: 20
                border.color: "black"
                border.width: 2
                color: heading ? "green" : "white"

                Text {
                    text: tabledata
                    font.pointSize: 10
                    anchors.centerIn: parent
                }
            }
        }

    }





}
