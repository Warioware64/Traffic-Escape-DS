BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= Traffic_Escape_DS
GAME_TITLE	:= Traffic Escape DS 
GAME_SUBTITLE	:= Resolves all your traffic problems!
GAME_AUTHOR	:= https://github.com/Warioware64


NITROFSDIR	:= nitrofiles


# Source code paths

AUDIODIRS	:= audio
BINDIRS 	:= bin
# Libraries

LIBS		:= -lnds9 -lmm9  -lNE
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
			$(BLOCKSDSEXT)/nitro-engine

CXXFLAGS	+= -fpermissive
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
