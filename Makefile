EXEC := berimbau-tool

CC := g++
CP := cp
MK := mkdir
RM := rm

SRCDIR := src
BUILDDIR := build
EXECDIR := bin

TARGET := $(EXECDIR)/$(EXEC)

SRCEXT := cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(addprefix $(BUILDDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))
CFLAGS := -g -Wall -std=c++17
LDFLAGS := -lstdc++fs -lpython2.7
INCLUDES := -I./$(SRCDIR)/include -std=c++17 -I/usr/include/python2.7

all: $(TARGET) cpmkspiffs

cpmkspiffs: libmkspiffs
	@echo "Copying spiffs to binary folder"
	@$(MK) -p $(EXECDIR)
	@$(CP) mkspiffs/mkspiffs $(EXECDIR)/mkspiffs

libmkspiffs:
	@make dist -C mkspiffs BUILD_CONFIG_NAME="-esp-idf" CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"

buildrun: all
	./$(TARGET)

run: cpmkspiffs
	./$(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(MK) -p $(EXECDIR)
	$(CC) $^ -o $(TARGET) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<"
	@$(MK) -p $(BUILDDIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

clean:
	@echo "Cleaning..."
	@$(RM) -rf $(BUILDDIR) $(EXECDIR) $(SHAREDIR)
	@make -C mkspiffs clean

.PHONY: clean