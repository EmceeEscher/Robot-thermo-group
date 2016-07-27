# settings for TINAH board
TINAH_DIR = $(HOME)/Documents/Arduino/hardware/TINAH/avr
BOARDS_TXT = $(TINAH_DIR)/boards.txt
ARDUINO_CORE_PATH = $(TINAH_DIR)/cores/Arduino
ARDUINO_VAR_PATH = $(TINAH_DIR)/variants
BOARD_TAG = TINAH
AVRDUDE_MCU = atmega128

# directory for output
OBJDIR = ../build-TINAH

# libraries to include
ARDUINO_LIBS = phys253TINAHArduino StandardCplusplus LiquidCrystal

# c++ standard
CXXFLAGS_STD = -std=c++11

# avrdude output
AVRDUDE_OPTS = -v -V

# external make file
HOMEBREW_CELLAR = $(shell brew --cellar)
include $(HOMEBREW_CELLAR)/arduino-mk/1.5.1/Arduino.mk
