QT += widgets concurrent
CONFIG += c++11
CONFIG -= console
TEMPLATE = app
TARGET = tividy-gui
!equals(QT_MAJOR_VERSION, 5): error("Use Qt 5.15; Qt 6 requires a newer C++ standard.")
lessThan(QT_MINOR_VERSION, 15): error("Qt 5.15 is required.")
INCLUDEPATH += $$PWD $$PWD/../include
SOURCES += $$PWD/main.cpp
GUI_FOLDERS = integration theme models controllers dialogs services views
for(folder, GUI_FOLDERS) {
    SOURCES += $$files($$PWD/$$folder/*.cpp)
    HEADERS += $$files($$PWD/$$folder/*.h)
}
SOURCES += $$PWD/../src/chain/Worker.cpp \
           $$PWD/../src/composite/WorkComponent.cpp \
           $$PWD/../src/composite/Item.cpp \
           $$PWD/../src/composite/ItemGroup.cpp \
           $$PWD/../src/command/WorkflowCommand.cpp \
           $$PWD/../src/command/Assign.cpp \
           $$PWD/../src/command/Cancel.cpp \
           $$PWD/../src/command/Escalate.cpp \
           $$PWD/../src/command/Progress.cpp \
           $$PWD/../src/command/Reject.cpp \
           $$PWD/../src/state/State.cpp \
           $$PWD/../src/state/Available.cpp \
           $$PWD/../src/state/InProgress.cpp \
           $$PWD/../src/state/Approved.cpp \
           $$PWD/../src/state/Completed.cpp \
           $$PWD/../src/state/Blocked.cpp \
           $$PWD/../src/state/Cancelled.cpp \
           $$PWD/../src/observer/Observer.cpp \
           $$PWD/../src/observer/EventSource.cpp \
           $$PWD/../src/iterator/Iterator.cpp \
           $$PWD/../src/iterator/DependencyIterator.cpp \
           $$PWD/../src/iterator/PriorityIterator.cpp \
           $$PWD/../src/strategy/AssignmentStrategy.cpp \
           $$PWD/../src/adapter/EmailService.cpp \
           $$PWD/../src/adapter/EmailServiceAdapter.cpp \
           $$PWD/../src/adapter/LegacyEmailService.cpp
DESTDIR = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/objects
MOC_DIR = $$OUT_PWD/moc
