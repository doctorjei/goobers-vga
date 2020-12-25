SOURCE_DIR = Source
BUILD_DIR = Build

MAIN_FILE = goobers.exe
PACK_FILE = datapack.exe

all:
	@cd $(SOURCE_DIR)
	@make
	@cd ..

clean:
	del $(BUILD_DIR)\$(MAIN_FILE)
	del $(BUILD_DIR)\$(PACK_FILE)
	del $(BUILD_DIR)\*.obj
