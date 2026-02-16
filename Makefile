BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/wonderful/thirdparty/blocksds/external

# User config

NAME		:= Traffic_Escape_DS
GAME_TITLE	:= Traffic Escape DS
GAME_SUBTITLE	:= Resolves all your traffic problems!
GAME_AUTHOR	:= https://github.com/Warioware64
GAME_ICON	:= icon.png

NITROFSDIR	:= nitrofiles


# Source code paths

AUDIODIRS	:= audio
BINDIRS 	:= bin
#GFXDIRS		:= graphics
# Libraries

# WiFi-enabled ARM7 binary (dswifi + maxmod audio)
ARM7ELF		:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS		:= -ldswifi9 -lnds9 -lmm9 -lNE -ldsf -lmbedcrypto -lmbedtls -lmbedx509 -lcurl -lz
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
			$(BLOCKSDS)/libs/dswifi \
			$(BLOCKSDSEXT)/libdsf \
			$(BLOCKSDSEXT)/nitro-engine \
			$(BLOCKSDSEXT)/mbedtls \
			$(BLOCKSDSEXT)/libcurl

CXXFLAGS	+= -fpermissive
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
