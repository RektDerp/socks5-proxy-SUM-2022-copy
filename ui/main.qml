import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TableModel 0.1

Window {
    width: 1020; height: 480
    visible: true
    title: qsTr("Proxy Statistics")

    Rectangle {
        anchors.fill: parent
        Row {
            id: header
            width: table.contentWidth
            height: 40
            x: -table.contentX
            z: 1
            spacing: 4
            Repeater {
                model: table.model.columnCount()
                Rectangle {
                    width: table.model.columnWidth(index); height: parent.height
                    color: "green"
                    border.color : "black"
                    border.width: 1

                    Text {
                        id: headerText
                        anchors.verticalCenter: parent.verticalCenter
                        x: 4
                        width: parent.width
                        text: table.model.headerData(index, Qt.Horizontal)
                    }
                }
            }
        }

        TableView {
            id: table
            anchors.fill: parent
            anchors.topMargin: header.height
            columnSpacing: 4; rowSpacing: 4
            model: TableModel { }
            //clip: true
            //ScrollIndicator.horizontal: ScrollIndicator { }
            ScrollIndicator.vertical: ScrollIndicator { }

            columnWidthProvider: function(column) {
                return model.columnWidth(column, Qt.font({family: "Arial", pixelSize: 20}));
            }

            //sortIndicatorVisible: true
            //onSortIndicatorColumnChanged: myModel.sort(sortIndicatorColumn, sortIndicatorOrder)
            //onSortIndicatorOrderChanged: myModel.sort(sortIndicatorColumn, sortIndicatorOrder)

            delegate: Rectangle {
                //implicitWidth: cellData.width + 20
                implicitHeight: cellData.height + 5
                color: "#EEE"

                Text {
                    id: cellData
                    width: parent.width
                    text: model.display
                    elide: Text.ElideRight
                    anchors.centerIn: parent
                    font.preferShaping: false
                }
            }
        }
    }
}
