import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SortFilterSessionModel 0.1
import "../content"

ApplicationWindow {
    title: qsTr("Proxy Statistics")
    width: 750; height: 480; visible: true

    header: ToolBar {
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

        TableView {
            id: table
            anchors.fill: parent
            anchors.topMargin: header.height
            columnSpacing: 4; rowSpacing: 4
            model: SortFilterSessionModel { }

            Timer {
                interval: sbUpdate.value * 1000
                repeat: true
                running: cbUpdate.checked
                onTriggered: table.model.sessionModel.update()
            }

            columnWidthProvider: function(column) {
                return model.columnWidth(column);
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
    }
}
