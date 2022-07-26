import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "../content"

Window {
    id: window
    title: qsTr("Proxy Statistics")
    width: 2 * sideMargin + table.tableViewWidth()
    height: 480; visible: true
    property int sideMargin: 10

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        spacing: sideMargin

        TimerBar {
            height: 30
            width: parent.width

            timer.onTriggered: {
                table.update()
                window.width = window.width - 1
                window.width = window.width + 1
            }
        }

        SessionTableView {
            id: table
            width: parent.width
            //height: parent.height / 2
            Layout.fillHeight: true
        }

        Rectangle {
            width: parent.width
            height: 125

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
                        width: table.adaptiveColumnWidth(0)
                        implicitHeight: 20

                        onTextChanged: {
                            table.userFilter = text
                        }
                    }

                    CalendarSpoiler {
                        id: createFilter
                        defaultText: "Create date"
                        width: table.adaptiveColumnWidth(1)
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
                        width: table.adaptiveColumnWidth(2)
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
                        width: table.adaptiveColumnWidth(3)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.isActiveFilter = text
                        }
                    }

                    TextField {
                        id: srcFilter
                        placeholderText : qsTr("Client")
                        width: table.adaptiveColumnWidth(4)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.srcEndpointFilter = text
                        }
                    }

                    TextField {
                        id: dstFilter
                        placeholderText : qsTr("Server")
                        width: table.adaptiveColumnWidth(5)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.dstEndpointFilter = text
                        }
                    }

                    TextField {
                        id: sentFilter
                        placeholderText : qsTr("Sent bytes")
                        width: table.adaptiveColumnWidth(6)
                        onTextEdited: table.contentY = 0
                        implicitHeight: 20
                        onTextChanged: {
                            table.bytesSentFilter = text.length > 0 ? text : "-1"
                        }
                    }
                    TextField {
                        id: receivedFilter
                        placeholderText : qsTr("Recv bytes")
                        width: table.adaptiveColumnWidth(7)
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
}
