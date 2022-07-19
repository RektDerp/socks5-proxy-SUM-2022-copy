import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SortFilterSessionModel 0.1
import "../content"

ApplicationWindow {
    title: qsTr("Proxy Statistics")
    width: 750; height: 480; visible: true

    header: ToolBar {
        width: parent.width
        RowLayout {
            anchors.fill: parent
            Switch {
                id: cbUpdate
                checked: true
                text: qsTr("Update every")
            }
            SpinBox {
                id: sbUpdate
                from: 1
                to: 60
                value: 2
                enabled: cbUpdate.checked
            }
            Label {
                text: "sec"
            }
            Item {
                Layout.fillWidth: true
            }
            TextField {
                id: tfFilter
                implicitWidth: parent.width / 4
                onTextEdited: table.contentY = 0
            }
        }
    }

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
                id: repeater
                model: table.model.columnCount()
                SortableColumnHeading {
                    width: table.model.columnWidth(index); height: parent.height
                    text: table.model.sessionModel.headerData(index, Qt.Horizontal)
                    initialSortOrder: table.model.initialSortOrder(index)
                    onSorting: {
                        for (var i = 0; i < repeater.model; ++i) {
                            if (i != index)
                                repeater.itemAt(i).stopSorting()
                            table.model.sort(index, state == "up" ? Qt.AscendingOrder : Qt.DescendingOrder)
                        }
                    }
                }
            }
        }

        SessionTableView {
            id: table
        }
    }
}
