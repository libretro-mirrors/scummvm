##LIBRETRO
DEBUG=0

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

ifeq ($(shell uname -a),)
   EXE_EXT = .exe
endif

TARGET_NAME := scummvm

LD        = $(CXX)
AR        = ar cru
RANLIB    = ranlib

ifeq ($(platform), unix)
   TARGET  := $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
# OS X
else ifeq ($(platform), osx)
   TARGET  := $(TARGET_NAME)_libretro.dylib
   DEFINES += -fPIC
   LDFLAGS += -dynamiclib -fPIC
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif

# iOS
else ifneq (,$(findstring ios,$(platform)))
   TARGET  := $(TARGET_NAME)_libretro_ios.dylib
   DEFINES += -fPIC -DHAVE_POSIX_MEMALIGN=1 -DIOS
   LDFLAGS += -dynamiclib -fPIC

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif

   CXX      = clang++ -arch armv7 -isysroot $(IOSSDK) -marm

ifeq ($(platform),ios9)
	CXX     += -miphoneos-version-min=8.0
else
	CXX     += -miphoneos-version-min=5.0
endif

# QNX
else ifeq ($(platform), qnx)
   TARGET  := $(TARGET_NAME)_libretro_qnx.so
   DEFINES += -fPIC -DSYSTEM_NOT_SUPPORTING_D_TYPE
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CC = qcc -Vgcc_ntoarmv7le
   CXX = QCC -Vgcc_ntoarmv7le
   LD = QCC -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le -A
   RANLIB="${QNX_HOST}/usr/bin/ntoarmv7-ranlib"

# PS3
else ifeq ($(platform), ps3)
   TARGET  := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   CXX = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-g++.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe rcs
   DEFINES += -DPLAYSTATION3
	STATIC_LINKING=1

# Nintendo Wii
else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_wii.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT) rcs
   DEFINES += -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__ -I$(DEVKITPRO)/libogc/include
	STATIC_LINKING=1

# Vita
else ifeq ($(platform), vita)
	TARGET := $(TARGET_NAME)_libretro_vita.a
	CC = arm-vita-eabi-gcc$(EXE_EXT)
	CXX = arm-vita-eabi-g++$(EXE_EXT)
	AR = arm-vita-eabi-ar$(EXE_EXT) rcs
	DEFINES += -DVITA
	STATIC_LINKING = 1

else ifeq ($(platform), android-armv7)
   TARGET  := $(TARGET_NAME)_libretro_android.so
   DEFINES += -fPIC -Wno-multichar -DUSE_VORBIS -DUSE_TREMOR -DUSE_ZLIB
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC -lz
   TOOLSET = arm-linux-androideabi-
   USE_TREMOR = 1
else ifneq (,$(findstring armv,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.so
   SHARED := -shared -Wl,--no-undefined
   DEFINES += -fPIC -Wno-multichar
   CC = gcc
ifneq (,$(findstring cortexa8,$(platform)))
   DEFINES += -marm -mcpu=cortex-a8
else ifneq (,$(findstring cortexa9,$(platform)))
   DEFINES += -marm -mcpu=cortex-a9
endif
   DEFINES += -marm
ifneq (,$(findstring neon,$(platform)))
   DEFINES += -mfpu=neon
   HAVE_NEON = 1
endif
ifneq (,$(findstring softfloat,$(platform)))
   DEFINES += -mfloat-abi=softfp
else ifneq (,$(findstring hardfloat,$(platform)))
   DEFINES += -mfloat-abi=hard
endif
   DEFINES += -DARM
else
	CC = gcc
   TARGET  := $(TARGET_NAME)_libretro.dll
   LDFLAGS += -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=../link.T
endif

ifeq ($(DEBUG), 1)
   DEFINES += -O0 -g
else
   DEFINES += -O3
endif

###SCUMM VM
srcdir = ../../../..
retrodir = backends/platform/libretro
LIBRETROCOMMON_DIR = $(retrodir)/libretro-common
VPATH = $(srcdir)

# Define toolset
ifdef TOOLSET
    CC        = $(TOOLSET)gcc
    CXX       = $(TOOLSET)g++
    LD        = $(TOOLSET)g++
    AR        = $(TOOLSET)ar cru
    RANLIB    = $(TOOLSET)ranlib
endif

MKDIR         = mkdir -p
RM            = rm -f
RM_REC        = rm -rf

# Define build flags
DEFINES       += -D__LIBRETRO__ -DNONSTANDARD_PORT -DUSE_RGB_COLOR -DUSE_OSD -DDISABLE_TEXT_CONSOLE -DFRONTEND_SUPPORTS_RGB565 -DUSE_MT32EMU -Wno-multichar
INCLUDES      += -I$(srcdir)/$(retrodir) -I. -I$(srcdir) -I$(srcdir)/engines -I$(srcdir)/$(LIBRETROCOMMON_DIR)/include
DEPDIR        = .deps
HAVE_GCC3     = true
USE_RGB_COLOR = true

OBJS := $(retrodir)/libretro.o \
        $(retrodir)/os.o \
		  $(srcdir)/$(LIBRETROCOMMON_DIR)/libco/libco.o

ifeq ($(USE_TREMOR), 1)
OBJS +=  $(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/bitwise.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/codebook.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/dsp.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/floor0.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/floor1.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/floor_lookup.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/framing.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/info.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/mapping0.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/mdct.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/misc.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/res012.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/tremor/vorbisfile.o
endif

ifneq ($(STATIC_LINKING), 1)
OBJS +=  $(srcdir)/$(LIBRETROCOMMON_DIR)/file/retro_dirent.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/file/retro_stat.o \
		  	$(srcdir)/$(LIBRETROCOMMON_DIR)/file/file_path.o \
			$(srcdir)/$(LIBRETROCOMMON_DIR)/compat/compat_strl.o
endif

POSIX = 1
DEFINES += -DFS_TYPE_POSIX
MODULE_DIRS += $(retrodir)

BACKEND := libretro
USE_MT32EMU = 1

include Makefile.common
