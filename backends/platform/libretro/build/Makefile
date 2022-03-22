##LIBRETRO
DEBUG=0
USE_ZLIB   = 1
USE_TREMOR = 0
USE_VORBIS = 1
USE_FLAC   = 1
USE_MAD    = 1
USE_FAAD   = 1
USE_PNG    = 1
USE_JPEG   = 1
USE_THEORADEC=1
USE_FREETYPE2=1
HAVE_MT32EMU=1
USE_FLUIDSYNTH=1
USE_LUA    = 1
USE_LIBCO  = 1

HIDE := @
SPACE :=
SPACE := $(SPACE) $(SPACE)
BACKSLASH :=
BACKSLASH := \$(BACKSLASH)
filter_out1 = $(filter-out $(firstword $1),$1)
filter_out2 = $(call filter_out1,$(call filter_out1,$1))
unixpath = $(subst \,/,$1)
unixcygpath = /$(subst :,,$(call unixpath,$1))

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
   arch = intel
ifeq ($(shell uname -p),arm64)
	arch = arm
endif
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
GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
	CXXFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

TARGET_64BIT = 0
ifeq ($(BUILD_64BIT),)
ifeq ($(shell uname -m), x86_64)
   BUILD_64BIT = 1
else ifeq ($(shell uname -m), arm64)
   BUILD_64BIT = 1
else
   BUILD_64BIT = 0
endif
endif

LD        = $(CXX)
AR        = ar cru
RANLIB    = ranlib

ifeq ($(platform), unix)
   TARGET  := $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   TARGET_64BIT := $(BUILD_64BIT)

# Raspberry Pi 3 (64 bit)
else ifeq ($(platform), rpi3_64)
   TARGET = $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   CFLAGS += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CFLAGS += -mcpu=cortex-a53 -mtune=cortex-a53
   CFLAGS += -fomit-frame-pointer -ffast-math
   CXXFLAGS = $(CFLAGS) -frtti
   BUILD_64BIT = 1

# Raspberry Pi 4 (64 bit)
else ifeq ($(platform), rpi4_64)
   TARGET = $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   CFLAGS += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CFLAGS += -mcpu=cortex-a72 -mtune=cortex-a72
   CFLAGS += -fomit-frame-pointer -ffast-math
   CXXFLAGS = $(CFLAGS) -frtti
   BUILD_64BIT = 1

# OS X
else ifeq ($(platform), osx)
   TARGET  := $(TARGET_NAME)_libretro.dylib
   DEFINES += -fPIC
   LDFLAGS += -dynamiclib -fPIC
   DEFINES += -DHAVE_POSIX_MEMALIGN=1
   TARGET_64BIT := $(BUILD_64BIT)

   ifeq ($(CROSS_COMPILE),1)
	TARGET_RULE   = -target $(LIBRETRO_APPLE_PLATFORM) -isysroot $(LIBRETRO_APPLE_ISYSROOT)
	CFLAGS   += $(TARGET_RULE)
	CPPFLAGS += $(TARGET_RULE)
	CXXFLAGS += $(TARGET_RULE)
	LDFLAGS  += $(TARGET_RULE)
	# Hardcode TARGET_64BIT for now
	TARGET_64BIT = 1
   endif

# iOS
else ifneq (,$(findstring ios,$(platform)))
   TARGET  := $(TARGET_NAME)_libretro_ios.dylib
   DEFINES += -fPIC -DHAVE_POSIX_MEMALIGN=1 -DIOS
   LDFLAGS += -dynamiclib -fPIC
   MINVERSION :=

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif
ifeq ($(platform),ios-arm64)
  CC        = cc -arch arm64 -isysroot $(IOSSDK)
  CXX       = c++ -arch arm64 -isysroot $(IOSSDK)
else
   CC       = cc -arch armv7 -isysroot $(IOSSDK)
   CXX      = c++ -arch armv7 -isysroot $(IOSSDK)
endif

ifeq ($(platform),$(filter $(platform),ios9 ios-arm64))
   MINVERSION += -miphoneos-version-min=8.0
else
   MINVERSION += -miphoneos-version-min=5.0
endif
  CFLAGS   += $(MINVERSION)
  CXXFLAGS += $(MINVERSION)

else ifeq ($(platform), tvos-arm64)
   EXT?=dylib
   TARGET := $(TARGET_NAME)_libretro_tvos.$(EXT)
   DEFINES += -fPIC -DHAVE_POSIX_MEMALIGN=1 -DIOS
   LDFLAGS += -dynamiclib -fPIC
ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk appletvos Path)
endif
   CC  = cc -arch arm64 -isysroot $(IOSSDK)
   CXX = c++ -arch arm64 -isysroot $(IOSSDK)

# QNX
else ifeq ($(platform), qnx)
   TARGET  := $(TARGET_NAME)_libretro_$(platform).so
   DEFINES += -fPIC -DSYSTEM_NOT_SUPPORTING_D_TYPE
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CC = qcc -Vgcc_ntoarmv7le
   CXX = QCC -Vgcc_ntoarmv7le
   LD = QCC -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le -A
   RANLIB="${QNX_HOST}/usr/bin/ntoarmv7-ranlib"

# Genode
else ifeq ($(platform), genode)
   TARGET  := libretro.so
   TARGET_64BIT := $(BUILD_64BIT)
   DEFINES += -fPIC -DSYSTEM_NOT_SUPPORTING_D_TYPE -DFRONTEND_SUPPORTS_RGB565
   C_PKGS   = libc
   CXX_PKGS = stdcxx genode-base
   CFLAGS   += -D__GENODE__ $(shell pkg-config --cflags $(C_PKGS))
   CXXFLAGS += -D__GENODE__ $(shell pkg-config --cflags $(CXX_PKGS))

   LIBS += $(shell pkg-config --libs $(C_PKGS) $(CXX_PKGS) genode-lib)

   CC  = $(shell pkg-config genode-base --variable=cc)
   CXX = $(shell pkg-config genode-base --variable=cxx)
   LD  = $(shell pkg-config genode-base --variable=ld)
   AR  = $(shell pkg-config genode-base --variable=ar) rcs
   RANLIB = genode-x86-ranlib

# Lightweight PS3 Homebrew SDK
else ifneq (,$(filter $(platform), ps3 psl1ght))
   TARGET  := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(PS3DEV)/ppu/bin/ppu-$(COMMONLV)gcc$(EXE_EXT)
   CXX = $(PS3DEV)/ppu/bin/ppu-$(COMMONLV)g++$(EXE_EXT)
   AR = $(PS3DEV)/ppu/bin/ppu-$(COMMONLV)ar$(EXE_EXT) rcs
   AR_NOPAR = $(PS3DEV)/ppu/bin/ppu-$(COMMONLV)ar$(EXE_EXT)
   DEFINES += -DPLAYSTATION3 -DWORDS_BIGENDIAN=1 -DMSB_FIRST -D__ppc__ -DNO_GETENV -D__PS3__
   STATIC_LINKING=1
   ifeq ($(platform), psl1ght)
        PLATFORM_DEFINES += -D__PSL1GHT__
   endif
   ifneq (,$(findstring MINGW32,$(shell uname -a)))
		PS3MINGW32 := 1
   endif
   LITE := 1

# Nintendo Wii
else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_wii.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT) rcs
   DEFINES += -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__ -I$(DEVKITPRO)/libogc/include
	STATIC_LINKING=1

# Nintendo Switch (libnx)
else ifeq ($(platform), libnx)
    export DEPSDIR := $(CURDIR)
    include $(DEVKITPRO)/libnx/switch_rules
    EXT=a
    TARGET := $(TARGET_NAME)_libretro_$(platform).$(EXT)
    DEFINES := -DSWITCH=1 -U__linux__ -U__linux
    DEFINES	+= -g -O3 -fPIE -I$(LIBNX)/include/ -ffunction-sections -fdata-sections -ftls-model=local-exec
    DEFINES += $(INCDIRS)
    DEFINES += -D__SWITCH__ -DHAVE_LIBNX -march=armv8-a -mtune=cortex-a57 -mtp=soft
    DEFINES += -I../libretro-common/include
    CXXFLAGS := $(ASFLAGS) -std=gnu++11 -fpermissive
    STATIC_LINKING = 1

# Nintendo Wii U
else ifeq ($(platform), wiiu)
   TARGET := $(TARGET_NAME)_libretro_wiiu.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT) rcs
   AR_ALONE = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   DEFINES += -DGEKKO -mwup -mcpu=750 -meabi -mhard-float -D__POWERPC__ -D__ppc__ -DWORDS_BIGENDIAN=1 -DMSB_FIRST
   DEFINES += -U__INT32_TYPE__ -U __UINT32_TYPE__ -D__INT32_TYPE__=int -fpermissive
   DEFINES += -DHAVE_STRTOUL -DWIIU -I../libretro-common/include
   LITE := 1
   CP := cp

else ifeq ($(platform), ctr)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITARM)/bin/arm-none-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITARM)/bin/arm-none-eabi-g++$(EXE_EXT)
   AR = $(DEVKITARM)/bin/arm-none-eabi-ar$(EXE_EXT) rcs
   RANLIB = $(DEVKITARM)/bin/arm-none-eabi-ranlib$(EXE_EXT)
   ifeq ($(strip $(CTRULIB)),)
      $(error "Please set CTRULIB in your environment. export CTRULIB=<path to>libctru")
   endif
   DEFINES += -DARM11 -D_3DS -I$(CTRULIB)/include
   DEFINES += -march=armv6k -mtune=mpcore -mfloat-abi=hard
   DEFINES += -Wall -mword-relocations
   DEFINES += -fomit-frame-pointer -ffast-math
   CXXFLAGS += -std=gnu++11 -fpermissive
   USE_VORBIS = 0
   USE_THEORADEC = 0
   USE_TREMOR = 1
   HAVE_MT32EMU = 0
   NO_HIGH_DEF := 1
   STATIC_LINKING = 1

# Vita
else ifeq ($(platform), vita)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = arm-vita-eabi-gcc$(EXE_EXT)
	CXX = arm-vita-eabi-g++$(EXE_EXT)
	AR = arm-vita-eabi-ar$(EXE_EXT) rcs
	DEFINES += -DVITA
	STATIC_LINKING = 1

# GCW0
else ifeq ($(platform), gcw0)
   TARGET := $(TARGET_NAME)_libretro.so
   CC = /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
   CXX = /opt/gcw0-toolchain/usr/bin/mipsel-linux-g++
   LD = /opt/gcw0-toolchain/usr/bin/mipsel-linux-g++
   AR = /opt/gcw0-toolchain/usr/bin/mipsel-linux-ar cru
   RANLIB = /opt/gcw0-toolchain/usr/bin/mipsel-linux-ranlib
   DEFINES += -DDINGUX -fomit-frame-pointer -ffast-math -march=mips32 -mtune=mips32r2 -mhard-float -fPIC
   DEFINES += -ffunction-sections -fdata-sections
   LDFLAGS += -shared -Wl,--gc-sections -Wl,--version-script=../link.T -fPIC
   USE_VORBIS = 0
   USE_THEORADEC = 0
   USE_TREMOR = 1
   USE_LIBCO  = 0
   HAVE_MT32EMU = 0
   NO_HIGH_DEF := 1

# MIYOO
else ifeq ($(platform), miyoo)
   TARGET := $(TARGET_NAME)_libretro.so
   CC = /opt/miyoo/usr/bin/arm-linux-gcc
   CXX = /opt/miyoo/usr/bin/arm-linux-g++
   LD = /opt/miyoo/usr/bin/arm-linux-g++
   AR = /opt/miyoo/usr/bin/arm-linux-ar cru
   RANLIB = /opt/miyoo/usr/bin/arm-linux-ranlib
   DEFINES += -DDINGUX -fomit-frame-pointer -ffast-math -march=armv5te -mtune=arm926ej-s -fPIC
   DEFINES += -ffunction-sections -fdata-sections
   LDFLAGS += -shared -Wl,--gc-sections -Wl,--version-script=../link.T -fPIC
   USE_VORBIS = 0
   USE_THEORADEC = 0
   USE_TREMOR = 1
   USE_LIBCO  = 0
   HAVE_MT32EMU = 0
   NO_HIGH_DEF := 1

else ifeq ($(platform), android-armv7)
   TARGET  := $(TARGET_NAME)_libretro_android.so
   DEFINES += -fPIC -Wno-multichar -D_ARM_ASSEM_
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   TOOLSET = arm-linux-androideabi-
   USE_VORBIS = 0
   USE_THEORADEC = 0
   USE_TREMOR = 1
   HAVE_MT32EMU = 0
else ifneq (,$(findstring armv,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC -Wno-multichar -D_ARM_ASSEM_
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   USE_VORBIS = 0
   USE_THEORADEC = 0
   USE_TREMOR = 1
   HAVE_MT32EMU = 0
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

# Emscripten
else ifeq ($(platform), emscripten)
	TARGET := $(TARGET_NAME)_libretro_$(platform).bc
	STATIC_LINKING = 1

# Windows MSVC 2017 all architectures
else ifneq (,$(findstring windows_msvc2017,$(platform)))

    NO_GCC := 1
    CFLAGS += -DNOMINMAX
    CXXFLAGS += -DNOMINMAX
    WINDOWS_VERSION = 1

	PlatformSuffix = $(subst windows_msvc2017_,,$(platform))
	ifneq (,$(findstring desktop,$(PlatformSuffix)))
		WinPartition = desktop
		MSVC2017CompileFlags = -DWINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP -FS
		LDFLAGS += -MANIFEST -LTCG:incremental -NXCOMPAT -DYNAMICBASE -DEBUG -OPT:REF -INCREMENTAL:NO -SUBSYSTEM:WINDOWS -MANIFESTUAC:"level='asInvoker' uiAccess='false'" -OPT:ICF -ERRORREPORT:PROMPT -NOLOGO -TLBID:1
		LIBS += kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib
	else ifneq (,$(findstring uwp,$(PlatformSuffix)))
		WinPartition = uwp
		MSVC2017CompileFlags = -DWINAPI_FAMILY=WINAPI_FAMILY_APP -D_WINDLL -D_UNICODE -DUNICODE -D__WRL_NO_DEFAULT_LIB__ -EHsc -FS
		LDFLAGS += -APPCONTAINER -NXCOMPAT -DYNAMICBASE -MANIFEST:NO -LTCG -OPT:REF -SUBSYSTEM:CONSOLE -MANIFESTUAC:NO -OPT:ICF -ERRORREPORT:PROMPT -NOLOGO -TLBID:1 -DEBUG:FULL -WINMD:NO
		LIBS += WindowsApp.lib
	endif

	CFLAGS += $(MSVC2017CompileFlags)
	CXXFLAGS += $(MSVC2017CompileFlags)

	TargetArchMoniker = $(subst $(WinPartition)_,,$(PlatformSuffix))

	CC  = cl.exe
	CXX = cl.exe
	LD = link.exe

	reg_query = $(call filter_out2,$(subst $2,,$(shell reg query "$2" -v "$1" 2>nul)))
	fix_path = $(subst $(SPACE),\ ,$(subst \,/,$1))

	ProgramFiles86w := $(shell cmd /c "echo %PROGRAMFILES(x86)%")
	ProgramFiles86 := $(shell cygpath "$(ProgramFiles86w)")

	WindowsSdkDir ?= $(call reg_query,InstallationFolder,HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0)
	WindowsSdkDir ?= $(call reg_query,InstallationFolder,HKEY_CURRENT_USER\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0)
	WindowsSdkDir ?= $(call reg_query,InstallationFolder,HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Microsoft SDKs\Windows\v10.0)
	WindowsSdkDir ?= $(call reg_query,InstallationFolder,HKEY_CURRENT_USER\SOFTWARE\Microsoft\Microsoft SDKs\Windows\v10.0)
	WindowsSdkDir := $(WindowsSdkDir)

	WindowsSDKVersion ?= $(firstword $(foreach folder,$(subst $(subst \,/,$(WindowsSdkDir)Include/),,$(wildcard $(call fix_path,$(WindowsSdkDir)Include\*))),$(if $(wildcard $(call fix_path,$(WindowsSdkDir)Include/$(folder)/um/Windows.h)),$(folder),)))$(BACKSLASH)
	WindowsSDKVersion := $(WindowsSDKVersion)

	VsInstallBuildTools = $(ProgramFiles86)/Microsoft Visual Studio/2017/BuildTools
	VsInstallEnterprise = $(ProgramFiles86)/Microsoft Visual Studio/2017/Enterprise
	VsInstallProfessional = $(ProgramFiles86)/Microsoft Visual Studio/2017/Professional
	VsInstallCommunity = $(ProgramFiles86)/Microsoft Visual Studio/2017/Community

	VsInstallRoot ?= $(shell if [ -d "$(VsInstallBuildTools)" ]; then echo "$(VsInstallBuildTools)"; fi)
	ifeq ($(VsInstallRoot), )
		VsInstallRoot = $(shell if [ -d "$(VsInstallEnterprise)" ]; then echo "$(VsInstallEnterprise)"; fi)
	endif
	ifeq ($(VsInstallRoot), )
		VsInstallRoot = $(shell if [ -d "$(VsInstallProfessional)" ]; then echo "$(VsInstallProfessional)"; fi)
	endif
	ifeq ($(VsInstallRoot), )
		VsInstallRoot = $(shell if [ -d "$(VsInstallCommunity)" ]; then echo "$(VsInstallCommunity)"; fi)
	endif
	VsInstallRoot := $(VsInstallRoot)

	VcCompilerToolsVer := $(shell cat "$(VsInstallRoot)/VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt" | grep -o '[0-9\.]*')
	VcCompilerToolsDir := $(VsInstallRoot)/VC/Tools/MSVC/$(VcCompilerToolsVer)

	WindowsSDKSharedIncludeDir := $(shell cygpath -w "$(WindowsSdkDir)\Include\$(WindowsSDKVersion)\shared")
	WindowsSDKUCRTIncludeDir := $(shell cygpath -w "$(WindowsSdkDir)\Include\$(WindowsSDKVersion)\ucrt")
	WindowsSDKUMIncludeDir := $(shell cygpath -w "$(WindowsSdkDir)\Include\$(WindowsSDKVersion)\um")
	WindowsSDKUCRTLibDir := $(shell cygpath -w "$(WindowsSdkDir)\Lib\$(WindowsSDKVersion)\ucrt\$(TargetArchMoniker)")
	WindowsSDKUMLibDir := $(shell cygpath -w "$(WindowsSdkDir)\Lib\$(WindowsSDKVersion)\um\$(TargetArchMoniker)")

	# For some reason the HostX86 compiler doesn't like compiling for x64
	# ("no such file" opening a shared library), and vice-versa.
	# Work around it for now by using the strictly x86 compiler for x86, and x64 for x64.
	# NOTE: What about ARM?
	ifneq (,$(findstring x64,$(TargetArchMoniker)))
		VCCompilerToolsBinDir := $(VcCompilerToolsDir)\bin\HostX64
	else
		VCCompilerToolsBinDir := $(VcCompilerToolsDir)\bin\HostX86
	endif

	PATH := $(shell IFS=$$'\n'; cygpath "$(VCCompilerToolsBinDir)/$(TargetArchMoniker)"):$(PATH)
	PATH := $(PATH):$(shell IFS=$$'\n'; cygpath "$(VsInstallRoot)/Common7/IDE")
	INCLUDE := $(shell IFS=$$'\n'; cygpath -w "$(VcCompilerToolsDir)/include")
	LIB := $(shell IFS=$$'\n'; cygpath -w "$(VcCompilerToolsDir)/lib/$(TargetArchMoniker)")
	ifneq (,$(findstring uwp,$(PlatformSuffix)))
		LIB := $(LIB);$(shell IFS=$$'\n'; cygpath -w "$(LIB)/store")
	endif
    
	export INCLUDE := $(INCLUDE);$(WindowsSDKSharedIncludeDir);$(WindowsSDKUCRTIncludeDir);$(WindowsSDKUMIncludeDir)
	export LIB := $(LIB);$(WindowsSDKUCRTLibDir);$(WindowsSDKUMLibDir)
	TARGET := $(TARGET_NAME)_libretro.dll
	PSS_STYLE :=2
	LDFLAGS += -DLL

else
	CC ?= gcc
	TARGET_64BIT := $(BUILD_64BIT)
	TARGET  := $(TARGET_NAME)_libretro.dll
	DEFINES += -DHAVE_FSEEKO -DHAVE_INTTYPES_H -fPIC
	CXXFLAGS += -fno-permissive
	LDFLAGS += -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=../link.T -fPIC
endif

ifeq ($(DEBUG), 1)
   DEFINES += -O0 -g
else ifeq ($(platform), wiiu)
   DEFINES += -Os
else ifeq ($(platform),genode)
   DEFINES += -O2
else ifneq (,$(findstring msvc,$(platform)))
   DEFINES += -O2
else
   DEFINES += -O3
endif

ifeq ($(TARGET_64BIT), 1)
   DEFINES += -DSIZEOF_SIZE_T=8 -DSCUMM_64BITS
else
   DEFINES += -DSIZEOF_SIZE_T=4
endif

ifeq ($(USE_LIBCO), 1)
   DEFINES += -DUSE_LIBCO
else
   LDFLAGS += -lpthread
endif

###SCUMM VM
CORE_DIR = ../../../..
srcdir   = $(CORE_DIR)
VPATH    = $(CORE_DIR)

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

ifeq ($(HAVE_MT32EMU),1)
USE_MT32EMU = 1
DEFINES += -DUSE_MT32EMU
endif

# Define build flags
DEFINES       += -D__LIBRETRO__ -DNONSTANDARD_PORT -DUSE_RGB_COLOR -DUSE_OSD -DDISABLE_TEXT_CONSOLE -DFRONTEND_SUPPORTS_RGB565
DEPDIR        = .deps
HAVE_GCC3     = true
USE_RGB_COLOR = true

DEFINES += -Wno-multichar -Wno-unknown-pragmas -Wno-write-strings
CXXFLAGS += -Wno-reorder

# Compile platform specific parts (e.g. filesystem)
ifeq ($(platform), win)
WIN32 = 1
endif

BACKEND := libretro

include Makefile.common

######################################################################
# The build rules follow - normally you should have no need to
# touch whatever comes after here.
######################################################################

# Concat DEFINES and INCLUDES to form the CPPFLAGS
CPPFLAGS := $(DEFINES) $(INCLUDES) $(INCFLAGS)
CXXFLAGS += $(DEFINES) $(INCLUDES)
CFLAGS += $(DEFINES) $(INCLUDES)

# Include the build instructions for all modules
-include $(addprefix $(CORE_DIR)/, $(addsuffix /module.mk,$(MODULES)))

# Depdir information
DEPDIRS = $(addsuffix $(DEPDIR),$(MODULE_DIRS))

# Hack for libnx DEPSDIR issues
libnx-ln:
ifeq ($(platform), libnx)
	ln -s ../../../../audio/ audio
	ln -s ../../../../backends/ backends
	ln -s ../../../../base/ base
	ln -s ../../../../common/ common
	ln -s ../../../../engines/ engines
	ln -s ../../../../graphics/ graphics
	ln -s ../../../../gui/ gui
	ln -s ../../../../image/ image
	ln -s ../../../../video/ video
	touch libnx-ln
endif

OBJOUT   = -o
LINKOUT  = -o 

ifneq (,$(findstring msvc,$(platform)))
	OBJOUT = -Fo
	LINKOUT = -out:
ifeq ($(STATIC_LINKING),1)
	LD ?= lib.exe
	STATIC_LINKING=0
else
	LD ?= link.exe
endif
endif

ifeq ($(platform), wiiu)
$(TARGET): $(OBJS) libdeps.a 
	$(MKDIR) libtemp
	$(CP) $+ libtemp/
	$(AR_ALONE) -M < lite_wiiu.mri
else ifeq ($(platform), libnx)
$(TARGET): libnx-ln $(OBJS) libdeps.a 
	$(MKDIR) libtemp
	cp $+ libtemp/
	$(AR) -M < libnx.mri
else ifeq ($(platform), ctr)
$(TARGET): $(OBJS) libdeps.a
	$(MKDIR) libtemp
	cp $+ libtemp/
	$(AR) -M < ctr.mri
else ifeq ($(PS3MINGW32), 1)
$(TARGET): $(OBJS) libdeps.a
	$(HIDE)echo Linking $@...
	$(HIDE)$(AR) $@ $(shell find ../ -maxdepth 1 -mindepth 1 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 2 -mindepth 2 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 3 -mindepth 3 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 4 -mindepth 4 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 5 -mindepth 5 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 6 -mindepth 6 -name '*.o')
	$(HIDE)$(AR_NOPAR) q $@ $(shell find ../ -maxdepth 7 -mindepth 7 -name '*.o')
else ifeq ($(STATIC_LINKING), 1)
$(TARGET): $(OBJS) libdeps.a 
	@echo Linking $@...
	$(HIDE)$(AR) $@ $(wildcard *.o) $(wildcard */*.o) $(wildcard */*/*.o) $(wildcard */*/*/*.o) $(wildcard */*/*/*/*.o)  $(wildcard */*/*/*/*/*.o)
else
$(TARGET): $(OBJS) libdeps.a 
	@echo Linking $@...
	$(HIDE)$(LD) $(LDFLAGS) $+ $(LIBS) $(LINKOUT)$@
endif

libdeps.a: $(OBJS_DEPS)
ifeq ($(platform), libnx)
		@echo Linking $@...
		$(HIDE)$(AR) -rc $@ $^
else
		@echo Linking $@...
		$(HIDE)$(AR) $@ $^
endif

%.o: %.c
	@echo Compiling $(<F)...
	@$(MKDIR) $(*D)
	$(HIDE)$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

%.o: %.cpp
	@echo Compiling $(<F)...
	@$(MKDIR) $(*D)
	$(HIDE)$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

# Dumb compile rule, for C++ compilers that don't allow dependency tracking or
# where it is broken (such as GCC 2.95).
.cpp.o:
	@$(MKDIR) $(*D)
	@echo Compiling $<...
	$(HIDE)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(<) $(OBJOUT)$*.o

clean:
	@echo Cleaning project...
	$(HIDE)$(RM_REC) $(DEPDIRS)
	$(HIDE)$(RM) $(OBJS) $(OBJS_DEPS) libdeps.a $(TARGET)
ifeq ($(platform), wiiu)
	$(HIDE)$(RM_REC) libtemp
endif	
ifeq ($(platform), libnx)
	$(HIDE)$(RM_REC) libtemp
	$(HIDE)$(RM) audio
	$(HIDE)$(RM) backends
	$(HIDE)$(RM) base
	$(HIDE)$(RM) common
	$(HIDE)$(RM) engines
	$(HIDE)$(RM) graphics
	$(HIDE)$(RM) gui
	$(HIDE)$(RM) image
	$(HIDE)$(RM) video
	$(HIDE)$(RM) libnx-ln
endif	

# Include the dependency tracking files.
-include $(wildcard $(addsuffix /*.d,$(DEPDIRS)))

# Mark *.d files and most *.mk files as PHONY. This stops make from trying to
# recreate them (which it can't), and in particular from looking for potential
# source files. This can save quite a bit of disk access time.
.PHONY: $(wildcard $(addsuffix /*.d,$(DEPDIRS))) $(addprefix $(CORE_DIR)/, $(addsuffix /module.mk,$(MODULES))) \
	$(CORE_DIR)/$(port_mk) $(CORE_DIR)/rules.mk $(CORE_DIR)/engines/engines.mk

.PHONY: all clean
