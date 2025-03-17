###############################################################################
# Makefile to compile code with arduino-cli for defined boards
# on the command line.
#
# Author: Dirk Selbach, luckynrslevin@users.noreply.github.com
# Inspired by: https://github.com/digiampietro/arduino-makefile/tree/master
# 
# Build targets:
# 1) Build all defined boards
#    make all
# 2) Build ELF for an individual board
#    make build/<short-name-of-the-board>/IoTBasic.ino.elf 
#    e.g. make build/d1_mini/IoTBasic.ino.elf to build ELF for d1_mini board
# 3) Remove build dir for all boards
#    make clean
# 4) Remove build dir for an individual board
#    TODO
#
# Adding additional boards:
# 1) Get FQBN name of the board you want to add: arduino-cli board listall
# 2) Add the FQBN to the list in FQBN variable below
# 3) Add short name (last part of FQBN) to the list in SBN variable below
#
###############################################################################

# Let's check if arduino-cli is installed
ifeq (, $(shell which arduino-cli))
$(error "arduino-cli command not found. Please install and add to PATH. See: https://arduino.github.io/arduino-cli/1.2/")
endif

OSFAMILY	:= $(shell ( uname | sed "s/-.*//" ))
MAKE_DIR	:= $(PWD)
GIT_VERSION	:= $(shell git describe --abbrev=4 --dirty --always --tags)

ifdef GIT_VERSION
  CFLAGS = --build-property compiler.cpp.extra_flags=-DMYVERSION=\"$(GIT_VERSION)\"
else
  CFLAGS =
endif

# Fully qualified board name
# For a full list of FQBNs see: arduino-cli board listall
# To add a board add the FQBN to the list and the sort name to the SBN list below.
FQBN	:= esp32:esp32:d1_mini32 esp8266:esp8266:d1_mini esp8266:esp8266:nodemcuv2

# board short names (=last part of the FQBN)
SBN	:= d1_mini32 d1_mini nodemcuv2

#  Rule to extract the FQBN based on the short name
get_full_name = $(filter %$(1),$(FQBN))

# Verbosity level, e.g., -v for verbose output
#VFLAG	:= -v
VFLAG	:=

# Sources and headers
SRCINO	:= $(wildcard *.ino)
SRC	:= $(wildcard *.ino *.c *.cpp mylib/*/*.ino)
HDRS	:= $(wildcard *.h mylib/*/*.h)

# Build directory (subdirs for the boards will be created within ELF build target)
BUILD_DIR	:= build

# Define the .elf output file for each board
ELF_FILES	:= $(addprefix $(BUILD_DIR)/,$(addsuffix /$(SRCINO).elf,$(SBN)))

$(info OSFAMILY     is [${OSFAMILY}])
$(info MAKE_DIR     is [${MAKE_DIR}])
$(info BUILD_DIR    is [${BUILD_DIR}])
$(info SRCINO       is [${SRCINO}])
$(info SRC          is [${SRC}])
$(info HDRS         is [${HDRS}])
$(info ELF_FILES    is [${ELF_FILES}])

# Default target to build for all boards
all: $(ELF_FILES)
.PHONY: all

# Build target for each board
$(ELF_FILES): $(SRC) $(HDRS)
#	echo "target: $@ prereq: $<"
	@echo "Building for board: $(call get_full_name,$(patsubst build/%/IoTBasic.ino.elf,%,$@))"
	@mkdir -p $(dir $@)
	arduino-cli compile -b $(call get_full_name,$(patsubst build/%/IoTBasic.ino.elf,%,$@)) --build-path $(dir $@) $(VFLAG) $(CFLAGS)

# Cleanup build dir (all boards)
clean:
	@echo "Cleaning the build directory completely"
	rm -rf $(BUILD_DIR)
.PHONY: clean

