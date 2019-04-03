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
GLADEFILES := $(wildcard $(DATADIR)/*.glade)

TARGET := $(EXECDIR)/$(EXEC)

SRCEXT 	:= cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(addprefix $(TMPDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))

TARGET 	 := $(BINDIR)/$(BIN)
CXXFLAGS := -g -Wall -std=c++17 -Wno-register `pkg-config --cflags gtkmm-3.0`
LDFLAGS  := -lstdc++fs -lpython2.7 `pkg-config --libs gtkmm-3.0` -pthread
INCLUDES := -I./$(INCDIR) -I/usr/include/python2.7

CPGLADE 	:= $(addprefix $(SHAREDIR)/,$(notdir $(GLADEFILES)))
LIBMKSPIFFS := $(LIBDIR)/mkspiffs

all: $(TARGET) $(CPGLADE) $(LIBMKSPIFFS)

$(LIBMKSPIFFS):
	@echo "Compiling mkspiffs..."
	@make dist -C mkspiffs BUILD_CONFIG_NAME="-esp-idf" CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"
	@echo "Copying spiffs to binary folder"
	@$(MK) -p $(LIBDIR)
	@$(CP) mkspiffs/mkspiffs $(LIBDIR)/mkspiffs

buildrun: all
	./$(TARGET)

run:
	./$(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(MK) -p $(BINDIR)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

$(TMPDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<"
	@$(MK) -p $(TMPDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(CPGLADE): $(GLADEFILES)
	@echo "Copying glade files..."
	@$(MK) -p $(SHAREDIR)
	@$(CP) $< $@

clean:
	@echo "Cleaning..."
	@$(RM) -rf $(BUILDDIR) $(BINDIR) $(SHAREDIR) $(TMPDIR)
	@make -C mkspiffs clean

.PHONY: clean