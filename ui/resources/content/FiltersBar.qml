import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    property SessionTableView table

    ColumnLayout {
        id: filters
        Text {
            height: 20
            width: 50
            text: "Filters"
        }
        Row {
            spacing: table.spacingpx
            TextField {
                id: userFilter
                placeholderText : qsTr("Username")
                width: table.adaptiveColumnWidth(0)
                implicitHeight: 20
                x: -table.contentX
                onTextChanged: {
                    table.userFilter = text
                }
            }

            CalendarButton {
                id: createFilter
                defaultText: "Create date"
                width: table.adaptiveColumnWidth(1)
                implicitHeight: 20
                x: userFilter.x + userFilter.width + table.spacingpx
                onDateChanged: {
                    table.createDateFilter = selectedDate
                }
                onClear: {
                    table.createDateFilter = new Date(0,0,0)
                }
            }

            CalendarButton {
                id: updateFilter
                defaultText: "Update date"
                width: table.adaptiveColumnWidth(2)
                x: createFilter.x + createFilter.width + table.spacingpx
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
                implicitHeight: 20
                x: updateFilter.x + updateFilter.width + table.spacingpx
                onTextChanged: {
                    table.isActiveFilter = text
                }
            }

            TextField {
                id: srcFilter
                placeholderText : qsTr("Client")
                width: table.adaptiveColumnWidth(4)
                implicitHeight: 20
                x: activeFilter.x + activeFilter.width + table.spacingpx
                onTextChanged: {
                    table.srcEndpointFilter = text
                }
            }

            TextField {
                id: dstFilter
                placeholderText : qsTr("Server")
                width: table.adaptiveColumnWidth(5)
                implicitHeight: 20
                x: srcFilter.x + srcFilter.width + table.spacingpx
                onTextChanged: {
                    table.dstEndpointFilter = text
                }
            }

            TextField {
                id: sentFilter
                placeholderText : qsTr("Sent bytes")
                width: table.adaptiveColumnWidth(6)
                implicitHeight: 20
                x: dstFilter.x + dstFilter.width + table.spacingpx
                onTextChanged: {
                    table.bytesSentFilter = text.length > 0 ? text : "-1"
                }
            }
            TextField {
                id: receivedFilter
                placeholderText : qsTr("Recv bytes")
                width: table.adaptiveColumnWidth(7)
                implicitHeight: 20
                x: sentFilter.x + sentFilter.width + table.spacingpx
                onTextChanged: {
                    table.bytesRecvFilter = text.length > 0 ? text : "-1"
                }
            }
        }

        Text {
            text: "By period:"
        }

        Row {
            spacing: 5

            Text {
                text: "From: "
                height: from.height
                verticalAlignment: Text.AlignVCenter
            }

            CalendarButton {
                id: from
                width: 80
                implicitHeight: 20
                defaultText: "[Not chosen]"

                onDateChanged: {
                    table.fromDateFilter = selectedDate
                }

                onClear: {
                    table.fromDateFilter = new Date(0,0,0)
                }
            }

            Text {
                height: to.height
                text: "To: "
                verticalAlignment: Text.AlignVCenter
            }

            CalendarButton {
                id: to
                width: 80
                implicitHeight: 20
                defaultText: "[Not chosen]"
                onDateChanged: {
                    table.toDateFilter = selectedDate
                }

                onClear: {
                    table.toDateFilter = new Date(0,0,0)
                }
            }

            Button {
                anchors.leftMargin: 50
                width: 80
                implicitHeight: 20
                text: "Reset filters"
                onClicked: {
                    userFilter.text = null
                    table.userFilter = null
                    createFilter.clear()
                    updateFilter.clear()
                    from.clear()
                    to.clear()
                    activeFilter.text = null
                    table.isActiveFilter = null
                    srcFilter.text = null
                    table.srcEndpointFilter = null
                    dstFilter.text = null
                    table.dstEndpointFilter= null
                    sentFilter.text = null
                    table.bytesSentFilter = -1
                    receivedFilter.text = null
                    table.bytesRecvFilter = -1
                }
            }
        }
    }
}
