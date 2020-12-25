CPP = tcc
SOURCE_DIR = Source
BUILD_DIR = ..\Build

MAIN_SRC_FILES = errors.cpp goobers.cpp mouse.cpp picture.cpp screen.cpp set.cpp tools.cpp vga.cpp window.cpp
PACK_SRC_FILES = datapack.cpp
MAIN_FILE = goobers.exe
PACK_FILE = datapack.exe

CPPFLAGS = -G -ml -n$(BUILD_DIR)

all: $(BUILD_DIR)\$(MAIN_FILE) $(BUILD_DIR)\$(PACK_FILE)


$(BUILD_DIR)\$(MAIN_FILE): $(MAIN_SRC_FILES)
	$(CPP) $(CPPFLAGS) -e$(MAIN_FILE) $?

$(BUILD_DIR)\$(PACK_FILE): $(PACK_SRC_FILES)
	$(CPP) $(CPPFLAGS) -e$(PACK_FILE) $?

clean:
	del $(BUILD_DIR)\$(MAIN_FILE)
	del $(BUILD_DIR)\$(PACK_FILE)
	del $(BUILD_DIR)\*.obj
