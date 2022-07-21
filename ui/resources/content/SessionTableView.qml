import QtQuick
import QtQuick.Controls
import SortFilterSessionModel 0.1

TableView {
    id: table
    property alias updateInterval: timer.interval
    property alias updateEnabled: timer.running
    property alias userFilter: tableModel.userFilter
    property alias createDateFilter: tableModel.createDateFilter
    property alias updateDateFilter: tableModel.updateDateFilter

    columnSpacing: 4; rowSpacing: 4
    model: SortFilterSessionModel { id: tableModel }

    Timer {
        id: timer
        repeat: true
        onTriggered: tableModel.sessionModel.update()
    }

    delegate: Rectangle {
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

    ScrollBar.horizontal: ScrollBar { }
    ScrollBar.vertical: ScrollBar { }
}
