CC = /usr/bin/i586-pc-msdosdjgpp-gcc
CPP = /usr/bin/i586-pc-msdosdjgpp-g++
CPPFLAGS = -fpermissive -w

RESOURCE_DIR = Resources
SOURCE_DIR = Source
BUILD_DIR = Build

MAIN_SRC_FILES = errors.cpp goobers.cpp mouse.cpp picture.cpp screen.cpp set.cpp tools.cpp vga.cpp window.cpp
MAIN_OBJ_FILES = $(MAIN_SRC_FILES:.cpp=.o)
MAIN_RES_DIRS = animate font menu misc
MAIN_FILE = goobers.exe

all: ${BUILD_DIR}/${MAIN_FILE} | $(addprefix ${BUILD_DIR}/, $(MAIN_RES_DIRS))

${BUILD_DIR}/%.o: ${SOURCE_DIR}/%.cpp | ${BUILD_DIR}
	${CPP} -c $(CPPFLAGS) $< -o $@

${BUILD_DIR}/${MAIN_FILE}: $(addprefix ${BUILD_DIR}/, $(MAIN_OBJ_FILES))
	${CPP} $(CPPFLAGS) $^ -o $@

${BUILD_DIR}:
	mkdir $@

$(addprefix ${BUILD_DIR}/, $(MAIN_RES_DIRS)): | ${BUILD_DIR}
	cp -r ${RESOURCE_DIR}/* ${BUILD_DIR}

clean:
	rm ${BUILD_DIR}/${MAIN_FILE} ${BUILD_DIR}/*.o
	rm -rf $(addprefix ${BUILD_DIR}/, $(MAIN_RES_DIRS))
