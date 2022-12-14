QT += quick sql core gui

SOURCES += \
        main.cpp \
    session.cpp \
	sessionmodel.cpp \
	sortfiltersessionmodel.cpp

resources.prefix = /$${TARGET}/resources
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
	session.h \
	sessionmodel.h \
	sortfiltersessionmodel.h

DISTFILES += \
	resources/content/CalendarButton.qml \
	resources/content/DatePicker.qml \
	resources/content/FiltersBar.qml \
	resources/content/TimerBar.qml
