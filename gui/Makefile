CXX ?= g++
PKG_CONFIG ?= pkg-config
MOC ?= moc
CORE_ROOT ?= ..
BUILD_DIR ?= build
TARGET ?= $(BUILD_DIR)/tividy-gui
QT_PACKAGES := Qt5Widgets Qt5Concurrent

CPPFLAGS += -I. -I$(CORE_ROOT)/include $(shell $(PKG_CONFIG) --cflags $(QT_PACKAGES) 2>/dev/null)
CXXFLAGS += -std=c++11 -Wall -Wextra -Wpedantic -fPIC -pthread -g
LDLIBS += $(shell $(PKG_CONFIG) --libs $(QT_PACKAGES) 2>/dev/null) -pthread

GUI_SOURCES := ./main.cpp $(addprefix ./,$(shell find integration theme models controllers dialogs services views -name '*.cpp'))
CORE_SOURCES := $(addprefix $(CORE_ROOT)/src/,\
    chain/Worker.cpp \
    composite/WorkComponent.cpp \
    composite/Item.cpp \
    composite/ItemGroup.cpp \
    command/WorkflowCommand.cpp \
    command/Assign.cpp \
    command/Cancel.cpp \
    command/Escalate.cpp \
    command/Progress.cpp \
    command/Reject.cpp \
    state/State.cpp \
    state/Available.cpp \
    state/InProgress.cpp \
    state/Approved.cpp \
    state/Completed.cpp \
    state/Blocked.cpp \
    state/Cancelled.cpp \
    observer/Observer.cpp \
    observer/EventSource.cpp \
    observer/Event.cpp \
    iterator/Iterator.cpp \
    iterator/DependencyIterator.cpp \
    iterator/PriorityIterator.cpp \
    strategy/AssignmentStrategy.cpp \
    adapter/EmailService.cpp \
    adapter/EmailServiceAdapter.cpp \
    adapter/LegacyEmailService.cpp)

GUI_OBJECTS := $(patsubst ./%.cpp,$(BUILD_DIR)/gui/%.o,$(GUI_SOURCES))
CORE_OBJECTS := $(patsubst $(CORE_ROOT)/src/%.cpp,$(BUILD_DIR)/core/%.o,$(CORE_SOURCES))
MOC_HEADERS := $(addprefix ./,$(shell find integration theme models controllers dialogs services views -name '*.h' -exec grep -l '^[[:space:]]*Q_OBJECT' {} +))
MOC_SOURCES := $(patsubst ./%.h,$(BUILD_DIR)/moc/%.cpp,$(MOC_HEADERS))
MOC_OBJECTS := $(MOC_SOURCES:.cpp=.o)
DEPS := $(GUI_OBJECTS:.o=.d) $(CORE_OBJECTS:.o=.d) $(MOC_OBJECTS:.o=.d)

.SECONDARY: $(MOC_SOURCES)

.PHONY: all run smoke check-deps clean
all: check-deps $(TARGET)

check-deps:
	@command -v $(PKG_CONFIG) >/dev/null || { echo "Install pkg-config and Qt 5 development packages; see README.md."; exit 1; }
	@$(PKG_CONFIG) --exists $(QT_PACKAGES) || { echo 'Install Qt 5 development packages and pkg-config; see README.md.'; exit 1; }
	@$(PKG_CONFIG) --atleast-version=5.15 Qt5Widgets || { echo 'Qt 5.15 or a later Qt 5 patch release is required.'; exit 1; }

$(TARGET): $(GUI_OBJECTS) $(CORE_OBJECTS) $(MOC_OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(GUI_OBJECTS) $(CORE_OBJECTS) $(MOC_SOURCES) $(MOC_OBJECTS): | check-deps

$(BUILD_DIR)/gui/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/core/%.o: $(CORE_ROOT)/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/moc/%.cpp: %.h
	@mkdir -p $(dir $@)
	$(MOC) $(CPPFLAGS) $< -o $@

$(BUILD_DIR)/moc/%.o: $(BUILD_DIR)/moc/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: all
	./$(TARGET)

smoke: all
	QT_QPA_PLATFORM=offscreen ./$(TARGET) --smoke

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)