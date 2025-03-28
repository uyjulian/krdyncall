#############################################
##                                         ##
##    Copyright (C) 2023-2023 Julian Uy    ##
##  https://sites.google.com/site/awertyb  ##
##                                         ##
##   See details of license at "LICENSE"   ##
##                                         ##
#############################################

CFLAGS += -std=gnu++17

SOURCES += tjsdyncall.cpp tjsdynuserdata.cpp CharacterSet.cpp

PROJECT_BASENAME = krdyncall

RC_URL ?= https://github.com/uyjulian/$(PROJECT_BASENAME)
RC_DESC ?= Dyncall plugin for TVP(KIRIKIRI) (2/Z)
RC_PRODUCTNAME ?= Dyncall for TVP(KIRIKIRI) (2/Z)
RC_LEGALCOPYRIGHT ?= Copyright (C) 2023-2023 Julian Uy; See details of license at license.txt, or the source code location.

include external/ncbind/Rules.lib.make
