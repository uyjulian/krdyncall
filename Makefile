#############################################
##                                         ##
##    Copyright (C) 2023-2023 Julian Uy    ##
##  https://sites.google.com/site/awertyb  ##
##                                         ##
##   See details of license at "LICENSE"   ##
##                                         ##
#############################################

CFLAGS += -std=gnu++17

BASESOURCES += tjsdyncall.cpp tjsdynload.cpp tjsdynguess.cpp tjsdynstruct.cpp tjsdyntype.cpp tjsdynuserdata.cpp CharacterSet.cpp
SOURCES += $(BASESOURCES)

PROJECT_BASENAME = krdyncall

RC_URL ?= https://github.com/uyjulian/$(PROJECT_BASENAME)
RC_DESC ?= Dyncall plugin for TVP(KIRIKIRI) (2/Z)
RC_PRODUCTNAME ?= Dyncall for TVP(KIRIKIRI) (2/Z)
RC_LEGALCOPYRIGHT ?= Copyright (C) 2023-2023 Julian Uy; See details of license at license.txt, or the source code location.

include external/ncbind/Rules.lib.make

DEPENDENCY_SOURCE_DIRECTORY_DYNCALL := $(DEPENDENCY_SOURCE_DIRECTORY)/dyncall

DEPENDENCY_SOURCE_FILE_DYNCALL := $(DEPENDENCY_SOURCE_DIRECTORY)/dyncall.tar.gz

DEPENDENCY_SOURCE_URL_DYNCALL := https://dyncall.org/r1.4/dyncall-1.4.tar.gz

$(DEPENDENCY_SOURCE_FILE_DYNCALL): | $(DEPENDENCY_SOURCE_DIRECTORY)
	curl --location --output $@ $(DEPENDENCY_SOURCE_URL_DYNCALL)

$(DEPENDENCY_SOURCE_DIRECTORY_DYNCALL): $(DEPENDENCY_SOURCE_FILE_DYNCALL)
	mkdir -p $@
	tar -x -f $< -C $@ --strip-components 1

DEPENDENCY_BUILD_DIRECTORY_DYNCALL := $(DEPENDENCY_BUILD_DIRECTORY)/dyncall

EXTLIBS += $(DEPENDENCY_OUTPUT_DIRECTORY)/lib/libdyncall_s.a $(DEPENDENCY_OUTPUT_DIRECTORY)/lib/libdynload_s.a
SOURCES += $(EXTLIBS)
OBJECTS += $(EXTLIBS)
LDLIBS += $(EXTLIBS) 

$(BASESOURCES): $(EXTLIBS)

$(DEPENDENCY_OUTPUT_DIRECTORY)/lib/libdyncall_s.a: | $(DEPENDENCY_SOURCE_DIRECTORY_DYNCALL) $(DEPENDENCY_SOURCE_DIRECTORY_SPIRV_TOOLS) $(DEPENDENCY_SOURCE_DIRECTORY_SPIRV_HEADERS) $(DEPENDENCY_SOURCE_DIRECTORY_GLSLANG) $(DEPENDENCY_OUTPUT_DIRECTORY)
	PKG_CONFIG_PATH=$(DEPENDENCY_OUTPUT_DIRECTORY)/lib/pkgconfig \
	cmake \
		-B $(DEPENDENCY_BUILD_DIRECTORY_DYNCALL) \
		-S $(DEPENDENCY_SOURCE_DIRECTORY_DYNCALL) \
		-DCMAKE_SYSTEM_NAME=Windows \
		-DCMAKE_SYSTEM_PROCESSOR=$(TARGET_CMAKE_SYSTEM_PROCESSOR) \
		-DCMAKE_FIND_ROOT_PATH=/dev/null \
		-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
		-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
		-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
		-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
		-DCMAKE_C_COMPILER=$(CC) \
		-DCMAKE_CXX_COMPILER=$(CXX) \
		-DCMAKE_RC_COMPILER=$(WINDRES) \
		-DCMAKE_INSTALL_PREFIX="$(DEPENDENCY_OUTPUT_DIRECTORY)" \
		-DCMAKE_BUILD_TYPE=Release \
		&& \
	cmake --build $(DEPENDENCY_BUILD_DIRECTORY_DYNCALL) && \
	cmake --build $(DEPENDENCY_BUILD_DIRECTORY_DYNCALL) --target install

$(DEPENDENCY_OUTPUT_DIRECTORY)/lib/libdynload_s.a: $(DEPENDENCY_OUTPUT_DIRECTORY)/lib/libdyncall_s.a


