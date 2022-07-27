import QtQuick 2.5
import QtQuick.Controls 2.5
import "../content/"

Button {
    property string defaultText: "Choose Date"
    property alias calendarVisible: calendar.visible
    property alias selectedDate: datePicker.selectedDate

    id: button
    text: defaultText

    onClicked: {
        calendar.visible = !calendar.visible
    }

    signal dateChanged;
    signal clear;

    onClear: {
        datePicker.set(new Date)
        button.text = defaultText
        calendar.visible = false
    }

    onDateChanged: {
        button.text = getDate(selectedDate)
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
                    Button {
                        height: 30
                        text: "Clear"
                        onClicked: {
                            clear()
                        }
                    }
                }

                DatePicker {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 200
                    height: 200
                    id: datePicker
                    onClicked: {
                        calendar.visible = false
                        dateChanged()
                    }
                }
            }
        }
    }

    function getDate(date) {
       return date.getFullYear()+'-'+(date.getMonth()+1)+'-'+date.getDate();
    }
}


