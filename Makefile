BIN := berimbau-tool-gui

CXX = g++
CP  := cp
MK  := mkdir
RM  := rm
PKGCFG := pkg-config

SRCDIR   := src
DATADIR	 := data
BUILDDIR = build
TMPDIR 	 := tmp
INCDIR 	 := $(SRCDIR)/include
BINDIR 	 := $(BUILDDIR)/bin
SHAREDIR := $(BUILDDIR)/share/$(BIN)
LIBDIR 	 := $(BUILDDIR)/lib/$(BIN)

SRCEXT   := cpp

SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
DATAFILES := $(wildcard $(DATADIR)/*.png) $(wildcard $(DATADIR)/*.glade)
SHAREFILES := $(addprefix $(SHAREDIR)/,$(notdir $(DATAFILES)))

$(info $(SHAREFILES))

TARGET := $(EXECDIR)/$(EXEC)

SRCEXT 	:= cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(addprefix $(TMPDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))

TARGET 	 := $(BINDIR)/$(BIN)
CXXFLAGS := -g -Wall -std=c++17 -Wno-register `$(PKGCFG) --cflags gtkmm-3.0 python-2.7`
LDFLAGS  := -lstdc++fs `$(PKGCFG) --libs gtkmm-3.0 python-2.7` -pthread
INCLUDES := -I./$(INCDIR)

ifeq ($(CXX),g++)
LIBMKSPIFFS := $(LIBDIR)/mkspiffs
else
LIBMKSPIFFS := $(LIBDIR)/mkspiffs.exe
endif

LIBESPTOOL  := $(LIBDIR)/esptool.py

all: $(TARGET) $(LIBMKSPIFFS) $(LIBESPTOOL) $(SHAREFILES)

$(LIBMKSPIFFS):
	@echo "Compiling mkspiffs..."
ifeq ($(CXX),g++)
	@make dist -C mkspiffs BUILD_CONFIG_NAME="-esp-idf" CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"
else
	make TARGET_OS=win32 CXX=x86_64-w64-mingw32-g++ CXXFLAGS=-fstack-protector LDFLAGS=-fstack-protector -C mkspiffs BUILD_CONFIG_NAME="-esp-idf" CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"
endif
	@echo "Copying spiffs to binary folder"
	@$(MK) -p $(LIBDIR)
ifeq ($(CXX),g++)
	@$(CP) mkspiffs/mkspiffs $(LIBDIR)/mkspiffs
else
	@$(CP) mkspiffs/mkspiffs.exe $(LIBDIR)/mkspiffs.exe
endif


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