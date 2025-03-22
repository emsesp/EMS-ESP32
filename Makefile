#
# GNUMakefile for EMS-ESP
#

_mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
I := $(patsubst %/,%,$(dir $(_mkfile_path)))

ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL = all
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      ECHO="COUNTTHIS" | grep -c "COUNTTHIS")
N := x
C = $(words $N)$(eval N := x $N)
ECHO = python3 $(I)/scripts/echo_progress.py --stepno=$C --nsteps=$T
endif

# determine number of parallel compiles based on OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    EXTRA_CPPFLAGS = -D LINUX
	JOBS ?= $(shell nproc)
endif
ifeq ($(UNAME_S),Darwin)
    EXTRA_CPPFLAGS = -D OSX -Wno-tautological-constant-out-of-range-compare
	JOBS ?= $(shell sysctl -n hw.ncpu)
endif
MAKEFLAGS += -j $(JOBS) -l $(JOBS)

# $(info Number of jobs: $(JOBS))

#----------------------------------------------------------------------
# Project Structure
#----------------------------------------------------------------------
# TARGET    is the name of the output
# BUILD     is the directory where object files & intermediate files will be placed
# SOURCES   is a list of directories containing source code
# INCLUDES  is a list of directories containing header files
# LIBRARIES is a list of directories containing libraries, this must be the top level containing include and lib
#----------------------------------------------------------------------
TARGET    := emsesp
BUILD     := build
SOURCES   := src/core src/devices src/web src/test       lib_standalone lib/semver lib/espMqttClient/src lib/espMqttClient/src/*         lib/ArduinoJson/src lib/uuid-common/src lib/uuid-console/src lib/uuid-log/src   lib/PButton 
INCLUDES  := src/core src/devices src/web src/test lib/* lib_standalone lib/semver lib/espMqttClient/src lib/espMqttClient/src/Transport lib/ArduinoJson/src lib/uuid-common/src lib/uuid-console/src lib/uuid-log/src   lib/uuid-telnet/src lib/uuid-syslog/src
LIBRARIES :=

CPPCHECK = cppcheck
CHECKFLAGS = -q --force --std=gnu++17

#----------------------------------------------------------------------
# Languages Standard
#----------------------------------------------------------------------
C_STANDARD   := -std=c17
CXX_STANDARD := -std=gnu++17

#----------------------------------------------------------------------
# Defined Symbols
#----------------------------------------------------------------------
DEFINES += -DARDUINOJSON_ENABLE -DARDUINOJSON_ENABLE_ARDUINO_STRING -DARDUINOJSON_USE_DOUBLE=0
DEFINES += -DEMSESP_STANDALONE -DEMSESP_TEST -DEMSESP_DEBUG -DEMC_RX_BUFFER_SIZE=1500
DEFINES += $(ARGS)

DEFAULTS = -DEMSESP_DEFAULT_LOCALE=\"en\" -DEMSESP_DEFAULT_TX_MODE=8 -DEMSESP_DEFAULT_VERSION=\"3.7.3-dev\" -DEMSESP_DEFAULT_BOARD_PROFILE=\"S32S3\"

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
CPPFLAGS  += -ggdb -g3 -O3
CPPFLAGS  += -MMD
CPPFLAGS  += -flto=auto -fno-lto
CPPFLAGS  += -Wall -Wextra -Werror
CPPFLAGS  += -Wswitch-enum
CPPFLAGS  += -Wno-unused-parameter
CPPFLAGS  += -Wno-missing-braces

CPPFLAGS  += $(EXTRA_CPPFLAGS)

CFLAGS    += $(CPPFLAGS)
CXXFLAGS  += $(CPPFLAGS)
LDFLAGS =

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
	@$(ECHO) Build complete.

$(OUTPUT): $(OBJS)
	@mkdir -p $(@D)
	@$(ECHO) Linking $@
	$(LINK.o)
	$(SYMBOLS.out)
	
$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	@$(ECHO) Compiling $@
	@$(COMPILE.c)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(ECHO) Compiling $@
	@$(COMPILE.cpp)

$(BUILD)/%.o: %.s
	@mkdir -p $(@D)
	@$(COMPILE.s)

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

endif
