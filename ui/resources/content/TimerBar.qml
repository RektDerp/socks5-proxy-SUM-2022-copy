import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    property alias timer: _timer

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

    Timer {
        id: _timer
        repeat: true
        interval: sbUpdate.value * 1000
        running: cbUpdate.checked
    }
}
