#
# GNUMakefile for EMS-ESP
#

NUMJOBS=${NUMJOBS:-" -j4 "}
MAKEFLAGS+="j "
#----------------------------------------------------------------------
# Project Structure
#----------------------------------------------------------------------
# TARGET    is the name of the output
# BUILD     is the directory where object files & intermediate files will be placed
# SOURCES   is a list of directories containing source code
# INCLUDES  is a list of directories containing header files
# LIBRARIES is a list of directories containing libraries, this must be the top level containing include and lib
#----------------------------------------------------------------------

#TARGET    := $(notdir $(CURDIR))
TARGET    := emsesp
BUILD     := build
SOURCES   := src src/* lib_standalone lib/uuid-common/src lib/uuid-console/src lib/uuid-log/src src/devices lib/ArduinoJson/src lib/PButton lib/semver lib/espMqttClient/src lib/espMqttClient/src/*
INCLUDES  := src lib_standalone lib/espMqttClient/src lib/espMqttClient/src/Transport lib/ArduinoJson/src lib/uuid-common/src lib/uuid-console/src lib/uuid-log/src lib/uuid-telnet/src lib/uuid-syslog/src lib/semver lib/* src/devices
LIBRARIES :=

CPPCHECK = cppcheck
# CHECKFLAGS = -q --force --std=c++17
CHECKFLAGS = -q --force --std=c++11

#----------------------------------------------------------------------
# Languages Standard
#----------------------------------------------------------------------
C_STANDARD   := -std=c17
# CXX_STANDARD := -std=c++17
CXX_STANDARD := -std=gnu++11

# C_STANDARD   := -std=c11
# CXX_STANDARD := -std=c++11

#----------------------------------------------------------------------
# Defined Symbols
#----------------------------------------------------------------------
DEFINES += -DARDUINOJSON_ENABLE_STD_STRING=1 -DARDUINOJSON_ENABLE_PROGMEM=1 -DARDUINOJSON_ENABLE_ARDUINO_STRING -DARDUINOJSON_USE_DOUBLE=0
DEFINES += -DEMSESP_DEBUG -DEMSESP_STANDALONE -DEMSESP_TEST -D__linux__ -DEMC_RX_BUFFER_SIZE=1500
DEFINES += $(ARGS)

DEFAULTS = -DEMSESP_DEFAULT_LOCALE=\"en\" -DEMSESP_DEFAULT_TX_MODE=8 -DEMSESP_DEFAULT_VERSION=\"3.6.0-dev\" -DEMSESP_DEFAULT_BOARD_PROFILE=\"S32\"

#----------------------------------------------------------------------
# Sources & Files
#----------------------------------------------------------------------
OUTPUT     := $(CURDIR)/$(TARGET)
SYMBOLS    := $(CURDIR)/$(BUILD)/$(TARGET).out

CSOURCES   := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CXXSOURCES := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))

OBJS       := $(patsubst %,$(BUILD)/%.o,$(basename $(CSOURCES)) $(basename $(CXXSOURCES)) )
DEPS       := $(patsubst %,$(BUILD)/%.d,$(basename $(CSOURCES)) $(basename $(CXXSOURCES)) )

INCLUDE    += $(addprefix -I,$(foreach dir,$(INCLUDES), $(wildcard $(dir))))
INCLUDE    += $(addprefix -I,$(foreach dir,$(LIBRARIES),$(wildcard $(dir)/include)))

LDLIBS     += $(addprefix -L,$(foreach dir,$(LIBRARIES),$(wildcard $(dir)/lib)))

#----------------------------------------------------------------------
# Compiler & Linker
#----------------------------------------------------------------------
CC  := /usr/bin/gcc
CXX := /usr/bin/g++

#----------------------------------------------------------------------
# Compiler & Linker Flags
#----------------------------------------------------------------------
# CPPFLAGS  C and C++ Compiler Flags
# CFLAGS    C Compiler Flags
# CXXFLAGS  C++ Compiler Flags
# LDFLAGS   Linker Flags
#----------------------------------------------------------------------
CPPFLAGS  += $(DEFINES) $(DEFAULTS) $(INCLUDE)
CPPFLAGS  += -ggdb
CPPFLAGS  += -g3
CPPFLAGS  += -Os

CFLAGS    += $(CPPFLAGS)
CFLAGS    += -Wall -Wextra -Werror -Wswitch-enum -Wno-unused-parameter -Wno-inconsistent-missing-override -Wno-missing-braces -Wno-unused-lambda-capture -Wno-sign-compare

CXXFLAGS  += $(CFLAGS) -MMD

#----------------------------------------------------------------------
# Compiler & Linker Commands
#----------------------------------------------------------------------
# LINK.o      link object files to binary
# COMPILE.c   compile C source files
# COMPILE.cpp compile C++ source files
#----------------------------------------------------------------------
ifeq ($(strip $(CXXSOURCES)),)
    LD := $(CC)
else
    LD := $(CXX)
endif

#DEPFLAGS   += -MF $(BUILD)/$*.d

LINK.o      = $(LD) $(LDFLAGS) $(LDLIBS) $^ -o $@
COMPILE.c   = $(CC) $(C_STANDARD) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
COMPILE.cpp = $(CXX) $(CXX_STANDARD) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

#----------------------------------------------------------------------
# Special Built-in Target
#----------------------------------------------------------------------
# .SUFFIXES     disable built-in wildcard rules
# .INTERMEDIATE make will treat targets as intermediate files, and delete them
# .PRECIOUS     make will not be deleted after it is no longer needed. Keep objects to speed up recompilation
# .PHONY        make will run this targets unconditionally, regardless of whether a file with that name exists or what its last-modification time is
#----------------------------------------------------------------------
.SUFFIXES:
.INTERMEDIATE:
.PRECIOUS: $(OBJS) $(DEPS)
.PHONY: all clean help

#----------------------------------------------------------------------
# Targets
#----------------------------------------------------------------------
.PHONY: all
.SILENT: $(OUTPUT)

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@mkdir -p $(@D)
	$(LINK.o)
	$(SYMBOLS.out)

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(COMPILE.c)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(COMPILE.cpp)

$(BUILD)/%.o: %.s
	@mkdir -p $(@D)
	$(COMPILE.s)

cppcheck: $(SOURCES)
	$(CPPCHECK) $(CHECKFLAGS) $^

run: $(OUTPUT)
	@$<

.PHONY: clean
clean:
	@$(RM) -rf $(BUILD) $(OUTPUT)

help:
	@echo available targets: all run clean
	@echo $(OUTPUT)

-include $(DEPS)