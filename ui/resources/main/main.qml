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
            Layout.preferredHeight: 30
            width: parent.width

            timer.onTriggered: {
                table.update()
                window.width = window.width - 1
                window.width = window.width + 1
            }
        }

        SessionTableView {
            id: table
            implicitWidth: parent.width
            Layout.fillHeight: true
        }

        FiltersBar {
            implicitWidth: parent.width
            Layout.preferredHeight: 110
            table: table
        }
    }
}
