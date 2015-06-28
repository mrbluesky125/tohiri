#
TARGET = harbour-tohiri

CONFIG += sailfishapp
QT += dbus
QT += multimedia

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

message($${DEFINES})

SOURCES += src/tohiri.cpp \
	src/tohir.cpp \
    src/driverBase.cpp \
    src/amg883x.cpp \
    src/colormap.cpp
	
HEADERS += src/tohir.h \
    src/driverBase.h \
    src/amg883x.h \
    src/colormap.h

OTHER_FILES += qml/tohiri.qml \
    qml/cover/CoverPage.qml \
    qml/pages/Tohiri.qml \
    qml/pages/aboutPage.qml \
    qml/pages/SettingsDialog.qml \
    harbour-tohiri.desktop \
    harbour-tohiri.png \
    qml/pages/Messagebox.qml \
    qml/pages/ColorBar.qml

