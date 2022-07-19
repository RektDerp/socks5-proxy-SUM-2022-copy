import QtQuick 2.0
import QtQuick.Controls
import "../content/"

Button {
    property string defaultText: "Choose Date"
    property alias calendarVisible: calendar.visible

    id: button
    text: defaultText

    onClicked: {
        calendar.visible = !calendar.visible
    }

    Rectangle {
        id: calendar
        visible: false
        anchors.bottom: button.top
        width: 220
        height: 255
        z: 1
        border.color: "black"
        border.width: 3
        color: "#EEE"

        Rectangle {
            anchors.centerIn: parent
            width: 200
            height: 235
            z: 1
            color: "#EEE"

            Column {
                spacing: 5
                z: 1

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10
                    Button {
                        height: 30
                        width: 30
                        text: "<"
                        onClicked: {
                            datePicker.prev()
                        }
                    }
                    Button {
                        height: 30
                        width: 30
                        text: ">"
                        onClicked: {
                            datePicker.next()
                        }
                    }
                }

                DatePicker {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 200
                    height: 200
                    id: datePicker
                    onClicked: {
                        button.text = defaultText + ": " + selectedDate
                        calendar.visible = false
                    }
                }
            }
        }
    }
}


