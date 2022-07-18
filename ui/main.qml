import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TableModel 0.1

Window {
    width: 1020; height: 480
    visible: true
    title: qsTr("Proxy Statistics")

    ColumnLayout {
        TableView {
            id: tableView
            width: 1020; height: 400
            columnSpacing: 4; rowSpacing: 4
            clip: true
            ScrollIndicator.horizontal: ScrollIndicator { }
            ScrollIndicator.vertical: ScrollIndicator { }
            model: TableModel { }
            //sortIndicatorVisible: true
            //onSortIndicatorColumnChanged: myModel.sort(sortIndicatorColumn, sortIndicatorOrder)
            //onSortIndicatorOrderChanged: myModel.sort(sortIndicatorColumn, sortIndicatorOrder)

            delegate: Rectangle {
                implicitWidth: cellData.width + 20
                implicitHeight: cellData.height + 20
                border.color: heading ? "black" : "white"
                border.width: 1
                color: heading ? "green" : "#EEE"

                Text {
                    id: cellData
                    text: tabledata
                    font.pointSize: 12
                    anchors.centerIn: parent
                }
            }
        }

    }
}
