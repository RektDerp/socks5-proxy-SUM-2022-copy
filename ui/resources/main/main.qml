import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "../content"

Window {
    id: window
    title: qsTr("Proxy Statistics")
    width: table.model.tableWidth() + scrollBarWidth + 2 * sideMargin + 4 * (table.model.columnCount() - 1);
    height: 480; visible: true
    property int sideMargin: 10
    property int scrollBarWidth: 20

    Column {
        anchors.fill: parent
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        spacing: sideMargin

        TimerBar {
            height: 30
            width: parent.width

            timer.onTriggered: {
                table.model.sessionModel.update()
                window.width = window.width - 1
                window.width = window.width + 1
            }
        }

        Row {
            id: header
            width: parent.width - scrollBarWidth - 2 * sideMargin
            height: 25
            x: -table.contentX
            z: 1
            spacing: 4

            Repeater {
                id: repeater
                model: table.model.columnCount()
                SortableColumnHeading {
                    id: heading
                    width: adaptiveColumnWidth(index)
                    height: parent.height
                    text: table.model.sessionModel.headerData(index, Qt.Horizontal)
                    initialSortOrder: table.model.initialSortOrder(index)

                    onSorting: {
                        table.contentY = 0
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
            anchors.topMargin: header.height
            width: parent.width
            height: parent.height / 2
            columnWidthProvider: function(column) { return repeater.itemAt(column).width }
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
                        placeholderText : qsTr("Username")
                        width: adaptiveColumnWidth(0)
                        implicitHeight: 20

                        onTextChanged: {
                            table.userFilter = text
                        }
                    }

                    CalendarSpoiler {
                        id: createFilter
                        defaultText: "Create date"
                        width: adaptiveColumnWidth(1)
                        implicitHeight: 20
                        onDateChanged: {
                            table.createDateFilter = selectedDate
                        }
                        onClear: {
                            table.createDateFilter = new Date(0,0,0)
                        }
                    }

                    CalendarSpoiler {
                        id: updateFilter
                        defaultText: "Update date"
                        width: adaptiveColumnWidth(2)
                        implicitHeight: 20

                        onDateChanged: {
                            table.updateDateFilter = selectedDate
                        }

                        onClear: {
                            table.updateDateFilter = new Date(0,0,0)
                        }
                    }

                    TextField {
                        id: activeFilter
                        placeholderText : qsTr("Active")
                        width: adaptiveColumnWidth(3)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.isActiveFilter = text
                        }
                    }

                    TextField {
                        id: srcFilter
                        placeholderText : qsTr("Client")
                        width: adaptiveColumnWidth(4)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.srcEndpointFilter = text
                        }
                    }

                    TextField {
                        id: dstFilter
                        placeholderText : qsTr("Server")
                        width: adaptiveColumnWidth(5)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.dstEndpointFilter = text
                        }
                    }

                    TextField {
                        id: sentFilter
                        placeholderText : qsTr("Sent bytes")
                        width: adaptiveColumnWidth(6)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.bytesSentFilter = text.length > 0 ? text : "-1"
                        }
                    }
                    TextField {
                        id: receivedFilter
                        placeholderText : qsTr("Recv bytes")
                        width: adaptiveColumnWidth(7)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.bytesRecvFilter = text.length > 0 ? text : "-1"
                        }
                    }
                }

                Text {
                    text: "By period:"
                }

                Column {
                    Row {
                        Text {
                            text: "From: "
                            height: from.height
                            verticalAlignment: Text.AlignVCenter
                        }

                        CalendarSpoiler {
                            id: from
                            defaultText: "[Not chosen]"

                            onDateChanged: {
                                table.fromDateFilter = selectedDate
                            }

                            onClear: {
                                table.fromDateFilter = new Date(0,0,0)
                            }
                        }
                    }
                    Row {
                        Text {
                            height: to.height
                            text: "To: "
                            verticalAlignment: Text.AlignVCenter
                        }
                        CalendarSpoiler {
                            id: to
                            defaultText: "[Not chosen]"

                            onDateChanged: {
                                table.toDateFilter = selectedDate
                            }

                            onClear: {
                                table.toDateFilter = new Date(0,0,0)
                            }
                        }
                    }
                }
            }
        }
    }



    function adaptiveColumnWidth(column) {
        return Math.max(table.model.columnWidth(column),
                        table.model.columnWidth(column) / table.model.tableWidth() * header.width);
    }
}
