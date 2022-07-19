import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SortFilterSessionModel 0.1
import "../content"

ApplicationWindow {
    title: qsTr("Proxy Statistics")
    width: 750; height: 480; visible: true
    property int sideMargin: 10
    color: "white"
    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        spacing: sideMargin


        Rectangle {
            Layout.preferredHeight: 30
            width: parent.width

            RowLayout {
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
            }
        }

        Rectangle {
            Layout.preferredHeight: parent.height - 230
            width: parent.width

            Row {
                id: header
                width: table.contentWidth
                height: 25
                x: -table.contentX
                z: 1
                spacing: 4
                Repeater {
                    id: repeater
                    model: table.model.columnCount()
                    SortableColumnHeading {
                        id: heading
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
                anchors.fill: parent
                anchors.topMargin: header.height
                updateInterval: sbUpdate.value * 1000
                updateEnabled: cbUpdate.checked
                filterText: userFilter.text
                columnWidthProvider: function(column) { return repeater.itemAt(column).width }
            }
        }

        Rectangle {
            width: parent.width
            height: 200

            ColumnLayout {
                id: filters
                Text {
                    height: 20
                    width: 50
                    text: "Filters"
                }
                Row {
                    spacing: 4


                    TextField {
                        id: userFilter
                        placeholderText : qsTr("User")
                        implicitWidth: table.model.columnWidth(0)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }

                    CalendarSpoiler {
                        id: createFilter
                        defaultText: "Create date"
                        implicitWidth: table.model.columnWidth(1)
                        implicitHeight: 20
                        //onTextEdited: table.contentY = 0 todo
                    }

                    CalendarSpoiler {
                        id: updateFilter
                        defaultText: "Update date"
                        implicitWidth: table.model.columnWidth(2)
                        implicitHeight: 20
                        //onTextEdited: table.contentY = 0 todo
                    }

                    TextField {
                        id: activeFilter
                        placeholderText : qsTr("Active")
                        implicitWidth: table.model.columnWidth(3)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }

                    TextField {
                        id: srcFilter
                        placeholderText : qsTr("Source")
                        implicitWidth: table.model.columnWidth(4)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }

                    TextField {
                        id: dstFilter
                        placeholderText : qsTr("Destination")
                        implicitWidth: table.model.columnWidth(5)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }

                    TextField {
                        id: sentFilter
                        placeholderText : qsTr("Sent bytes")
                        implicitWidth: table.model.columnWidth(6)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }
                    TextField {
                        id: receivedFilter
                        placeholderText : qsTr("Receive bytes")
                        implicitWidth: table.model.columnWidth(7)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                    }
                }

                Text {
                    text: "By period:"
                }

                Column {
                    CalendarSpoiler {
                        id: from
                        defaultText: "From"
                    }

                    CalendarSpoiler {
                        id: to
                        defaultText: "To"
                    }
                }
            }
        }
    }
}
