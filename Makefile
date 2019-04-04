BIN := berimbau-tool-gui

CXX := g++
CP  := cp
MK  := mkdir
RM  := rm

SRCDIR   := src
DATADIR	 := data
BUILDDIR := build
TMPDIR 	 := tmp
INCDIR 	 := $(SRCDIR)/include
BINDIR 	 := $(BUILDDIR)/bin
SHAREDIR := $(BUILDDIR)/share/$(BIN)
LIBDIR 	 := $(BUILDDIR)/lib/$(BIN)

SRCEXT   := cpp

SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
SHAREFILES := $(wildcard $(DATADIR)/*)

TARGET := $(EXECDIR)/$(EXEC)

SRCEXT 	:= cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(addprefix $(TMPDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))

TARGET 	 := $(BINDIR)/$(BIN)
CXXFLAGS := -g -Wall -std=c++17 -Wno-register `pkg-config --cflags gtkmm-3.0`
LDFLAGS  := -lstdc++fs -lpython2.7 `pkg-config --libs gtkmm-3.0` -pthread
INCLUDES := -I./$(INCDIR) -I/usr/include/python2.7

LIBMKSPIFFS := $(LIBDIR)/mkspiffs
LIBESPTOOL  := $(LIBDIR)/esptool.py

all: $(TARGET) $(CPGLADE) $(LIBMKSPIFFS) $(LIBESPTOOL)

$(LIBMKSPIFFS):
	@echo "Compiling mkspiffs..."
	@make dist -C mkspiffs BUILD_CONFIG_NAME="-esp-idf" CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"
	@echo "Copying spiffs to binary folder"
	@$(MK) -p $(LIBDIR)
	@$(CP) mkspiffs/mkspiffs $(LIBDIR)/mkspiffs

$(LIBESPTOOL):
	@echo "Copying esptool..."
	@$(MK) -p $(LIBDIR)
	@$(CP) esptool/esptool.py $(LIBDIR)/esptool.py

buildrun: all
	./$(TARGET)

run:
	./$(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(MK) -p $(BINDIR)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

$(TMPDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<..."
	@$(MK) -p $(TMPDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(SHAREDIR)/%: $(DATADIR)/%
	@echo "Copying $<..."
	@$(MK) -p $(SHAREDIR)
	@$(CP) $< $@

clean:
	@echo "Cleaning..."
	@$(RM) -rf $(BUILDDIR) $(BINDIR) $(SHAREDIR) $(TMPDIR)
	@make -C mkspiffs clean

.PHONY: clean