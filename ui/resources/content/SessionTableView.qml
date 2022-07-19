import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SortFilterSessionModel 0.1
import "../content"

TableView {
    anchors.fill: parent
    anchors.topMargin: header.height
    columnSpacing: 4; rowSpacing: 4
    model: SortFilterSessionModel {
        filterText: tfFilter.text
    }

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
