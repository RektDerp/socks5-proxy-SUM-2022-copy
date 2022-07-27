import QtQuick 2.15
import QtQuick.Controls 2.15
import SortFilterSessionModel 0.1

Item {
    property alias userFilter: tableModel.userFilter
    property alias createDateFilter: tableModel.createDateFilter
    property alias updateDateFilter: tableModel.updateDateFilter
    property alias fromDateFilter: tableModel.fromDateFilter
    property alias toDateFilter: tableModel.toDateFilter
    property alias isActiveFilter: tableModel.isActiveFilter
    property alias srcEndpointFilter: tableModel.srcEndpointFilter
    property alias dstEndpointFilter: tableModel.dstEndpointFilter
    property alias bytesSentFilter: tableModel.bytesSentFilter
    property alias bytesRecvFilter: tableModel.bytesRecvFilter
    property alias contentY: tableView.contentY
    property alias contentX: tableView.contentX
    property int scrollBarWidth: 40
    property int spacingpx: 4

    function adaptiveColumnWidth(column) {
        return Math.max(tableModel.columnWidth(column),
                        tableModel.columnWidth(column) / tableModel.tableWidth() * header.width);
    }

    function tableViewWidth() {
        return tableModel.tableWidth() + scrollBarWidth +  4 * (tableModel.columnCount() - 1)
    }

    function update() {
        tableModel.sessionModel.update()
    }

    Row {
        id: header
        width: parent.width - scrollBarWidth
        height: 25
        x: -tableView.contentX
        z: 1
        spacing: spacingpx

        Repeater {
            id: repeater
            model: tableView.model.columnCount()
            SortableColumnHeading {
                id: heading
                width: adaptiveColumnWidth(index)
                height: parent.height
                text: tableModel.sessionModel.headerData(index, Qt.Horizontal)
                initialSortOrder: tableModel.initialSortOrder(index)

                onSorting: {
                    tableView.contentY = 0
                    for (var i = 0; i < repeater.model; ++i) {
                        if (i != index)
                            repeater.itemAt(i).stopSorting()
                        tableView.model.sort(index, state == "up" ? Qt.AscendingOrder : Qt.DescendingOrder)
                    }
                }
            }
        }
    }

    TableView {
        id: tableView
        anchors.topMargin: header.height
        anchors.fill: parent
        columnSpacing: spacingpx; rowSpacing: spacingpx
        model: SortFilterSessionModel {
            id: tableModel
            onFilterChanged: {
                tableView.contentY = 0 // scroll to the top
            }
        }

        delegate: Rectangle {
            implicitHeight: cellData.height + 5
            color: "#EEE"

            Text {
                id: cellData
                width: parent.width - 6
                text: model.display
                elide: Text.ElideRight
                anchors.centerIn: parent
                font.preferShaping: false
            }
        }

        columnWidthProvider: function(column) { return repeater.itemAt(column).width }

        ScrollBar.horizontal: ScrollBar { }
        ScrollBar.vertical: ScrollBar { }
    }
}
