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
ifeq ($(shell uname -m), x86_64)
   BUILD_64BIT = 1
else
   BUILD_64BIT = 0
endif

LD        = $(CXX)
AR        = ar cru
RANLIB    = ranlib

ifeq ($(platform), unix)
   TARGET  := $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   TARGET_64BIT := $(BUILD_64BIT)
# OS X
else ifeq ($(platform), osx)
   TARGET  := $(TARGET_NAME)_libretro.dylib
   DEFINES += -fPIC
   LDFLAGS += -dynamiclib -fPIC
ifneq ($(shell uname -p),powerpc)
   arch = intel
   TARGET_64BIT := $(BUILD_64BIT)
else
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
   TARGET  := $(TARGET_NAME)_libretro_$(platform).so
   DEFINES += -fPIC -DSYSTEM_NOT_SUPPORTING_D_TYPE
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CC = qcc -Vgcc_ntoarmv7le
   CXX = QCC -Vgcc_ntoarmv7le
   LD = QCC -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le -A
   RANLIB="${QNX_HOST}/usr/bin/ntoarmv7-ranlib"

# PS3
else ifeq ($(platform), ps3)
   TARGET  := $(TARGET_NAME)_libretro_$(platform).a
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

# Nintendo Switch (libnx)
else ifeq ($(platform), libnx)
    export DEPSDIR := $(CURDIR)
    include $(DEVKITPRO)/libnx/switch_rules
    EXT=a
    TARGET := $(TARGET_NAME)_libretro_$(platform).$(EXT)
    DEFINES := -DSWITCH=1 -U__linux__ -U__linux
    DEFINES	:=	 -g -O3 \
                 -fPIE -I$(LIBNX)/include/ -ffunction-sections -fdata-sections -ftls-model=local-exec -Wl,--allow-multiple-definition -specs=$(LIBNX)/switch.specs
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
# Vita
else ifeq ($(platform), vita)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = arm-vita-eabi-gcc$(EXE_EXT)
	CXX = arm-vita-eabi-g++$(EXE_EXT)
	AR = arm-vita-eabi-ar$(EXE_EXT) rcs
	DEFINES += -DVITA
	STATIC_LINKING = 1

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
   CC = gcc
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
	CC = gcc
	TARGET_64BIT := $(BUILD_64BIT)
   TARGET  := $(TARGET_NAME)_libretro.dll
   DEFINES += -DHAVE_FSEEKO -DHAVE_INTTYPES_H -fno-permissive
   LDFLAGS += -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=../link.T
endif

ifeq ($(DEBUG), 1)
   DEFINES += -O0 -g
else ifeq ($(platform), wiiu)
   DEFINES += -Os
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
	LD = link.exe
endif
else
	LD = $(CXX)
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
else ifeq ($(STATIC_LINKING), 1)
$(TARGET): $(OBJS) libdeps.a 
	$(AR) $@ $(wildcard *.o) $(wildcard */*.o) $(wildcard */*/*.o) $(wildcard */*/*/*.o) $(wildcard */*/*/*/*.o)  $(wildcard */*/*/*/*/*.o)
else
$(TARGET): $(OBJS) libdeps.a 
	$(LD) $(LDFLAGS) $+ $(LIBS) $(LINKOUT)$@
endif

libdeps.a: $(OBJS_DEPS)
ifeq ($(platform), libnx)
		$(AR) -rc $@ $^
else
		$(AR) $@ $^
endif

# Dumb compile rule, for C++ compilers that don't allow dependency tracking or
# where it is broken (such as GCC 2.95).
.cpp.o:
	$(MKDIR) $(*D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(<) $(OBJOUT)$*.o

clean:
	$(RM_REC) $(DEPDIRS)
	$(RM) $(OBJS) $(OBJS_DEPS) libdeps.a $(TARGET)
ifeq ($(platform), wiiu)
	$(RM_REC) libtemp
endif	
ifeq ($(platform), libnx)
	$(RM_REC) libtemp
	$(RM) audio
	$(RM) backends
	$(RM) base
	$(RM) common
	$(RM) engines
	$(RM) graphics
	$(RM) gui
	$(RM) image
	$(RM) video
	$(RM) libnx-ln
endif	

# Include the dependency tracking files.
-include $(wildcard $(addsuffix /*.d,$(DEPDIRS)))

# Mark *.d files and most *.mk files as PHONY. This stops make from trying to
# recreate them (which it can't), and in particular from looking for potential
# source files. This can save quite a bit of disk access time.
.PHONY: $(wildcard $(addsuffix /*.d,$(DEPDIRS))) $(addprefix $(CORE_DIR)/, $(addsuffix /module.mk,$(MODULES))) \
	$(CORE_DIR)/$(port_mk) $(CORE_DIR)/rules.mk $(CORE_DIR)/engines/engines.mk

.PHONY: all clean
