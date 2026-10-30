QT += widgets concurrent
CONFIG += c++11
CONFIG -= console
TEMPLATE = app
TARGET = tividy-gui
!equals(QT_MAJOR_VERSION, 5): error("Use Qt 5.15; Qt 6 requires a newer C++ standard.")
lessThan(QT_MINOR_VERSION, 15): error("Qt 5.15 is required.")
INCLUDEPATH += $$PWD $$PWD/../include $$PWD/third_party
SOURCES += $$PWD/main.cpp $$PWD/../src/WorkflowManager.cpp
GUI_FOLDERS = integration theme models controllers dialogs services views
for(folder, GUI_FOLDERS) {
    SOURCES += $$files($$PWD/$$folder/*.cpp)
    HEADERS += $$files($$PWD/$$folder/*.h)
}
CORE_FOLDERS = adapter chain command composite factory iterator observer state strategy
for(folder, CORE_FOLDERS) {
    SOURCES += $$files($$PWD/../src/$$folder/*.cpp)
}
DESTDIR = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/objects
MOC_DIR = $$OUT_PWD/moc
