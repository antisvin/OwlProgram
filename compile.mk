BUILDROOT ?= .

C_SRC   = basicmaths.c heap_5.c fastpow.c fastlog.c # sbrk.c
CPP_SRC = main.cpp operators.cpp message.cpp system_tables.cpp
CPP_SRC += Patch.cpp PatchProcessor.cpp
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp ComplexShortArray.cpp FastFourierTransform.cpp ShortFastFourierTransform.cpp 
CPP_SRC += ShortArray.cpp
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp Window.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += SmoothValue.cpp PatchParameter.cpp
CPP_SRC += MonochromeScreenPatch.cpp ColourScreenPatch.cpp
CPP_SRC += Resource.cpp
C_SRC += font.c

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
DAISYSP      = $(BUILDROOT)/Libraries/DaisySP/Source
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -I$(DAISYSP)
CPPFLAGS += -I$(DAISYSP)/Control
CPPFLAGS += -I$(DAISYSP)/Drums
CPPFLAGS += -I$(DAISYSP)/Dynamics
CPPFLAGS += -I$(DAISYSP)/Effects
CPPFLAGS += -I$(DAISYSP)/Filters
CPPFLAGS += -I$(DAISYSP)/Noise
CPPFLAGS += -I$(DAISYSP)/PhysicalModeling
CPPFLAGS += -I$(DAISYSP)/Synthesis
CPPFLAGS += -I$(DAISYSP)/Utility
PATCH_C_SRC    = $(wildcard $(PATCHSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_CPP_SRC += PatchProgram.cpp
PATCH_C_SRC   += $(wildcard $(GENSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(GENSOURCE)/*.cpp)
ifdef MAXIMILIAN
PATCH_CPP_SRC := $(filter-out $(PATCHSOURCE)/$(MAXIMILIAN).cpp, $(PATCH_CPP_SRC))
endif
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))
PATCH_OBJS += $(BUILD)/startup.o
DAISYSP_CPP_SRC = $(wildcard $(DAISYSP)/*/*.cpp)
DAISYSP_OBJS = $(addprefix $(BUILD)/, $(notdir $(DAISYSP_CPP_SRC:.cpp=.o)))

CPPFLAGS += -DARM_CORTEX
CPPFLAGS += -DEXTERNAL_SRAM
CPPFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding

ifeq ($(PLATFORM), Daisy)
CPPFLAGS += -mtune=cortex-m7
CPPFLAGS += -DOWL_DAISY
else
CPPFLAGS += -mtune=cortex-m4
endif

CPPFLAGS += -fpic
CPPFLAGS += -fpie
CPPFLAGS += -fdata-sections
CPPFLAGS += -ffunction-sections
# CPPFLAGS += -munaligned-access
CPPFLAGS += -mno-unaligned-access
# CPPFLAGS += -mlong-calls

# CPPFLAGS += -mpic-data-is-text-relative
CPPFLAGS += -fno-omit-frame-pointer
CPPFLAGS += -flto

LDLIBS   = -lm
LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

CXXFLAGS = -fno-rtti
CXXFLAGS += -fno-exceptions

ifdef HEAVY
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
endif

CC=gcc
LD=gcc
AR=ar
AS=as
NM=nm
CXX=g++
GDB=gdb
SIZE=size
RANLIB=ranlib
OBJCOPY=objcopy
OBJDUMP=objdump

# object files
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)
OBJS += $(BUILD)/libnosys_gnu.o

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/common.mk

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)
vpath %.cpp $(DAISYSP)/Control
vpath %.cpp $(DAISYSP)/Drums
vpath %.cpp $(DAISYSP)/Dynamics
vpath %.cpp $(DAISYSP)/Effects
vpath %.cpp $(DAISYSP)/Filters
vpath %.cpp $(DAISYSP)/Noise
vpath %.cpp $(DAISYSP)/PhysicalModeling
vpath %.cpp $(DAISYSP)/Synthesis
vpath %.cpp $(DAISYSP)/Utility

vpath %.c Libraries/syscalls

.PHONY: libs as map compile

$(BUILD)/ShortPatchProgram.o: $(SOURCE)/ShortPatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp > $(@:.o=.d)

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

Libraries/libowlprg.a: $(OBJS)
	@$(AR) rcs $@ $^

Libraries/libdaisysp.a: $(DAISYSP_OBJS)
	@$(AR) rcs $@ $^

$(BUILD)/$(TARGET).elf: $(PATCH_OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(LDLIBS) Libraries/libdaisysp.a Libraries/libowlprg.a

libs: Libraries/libowlprg.a Libraries/libdaisysp.a

as: $(BUILD)/$(TARGET).elf
	@$(OBJDUMP) -S $< > $(BUILD)/$(TARGET).s

map: $(PATCH_OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/$(TARGET).map $(PATCH_OBJS) $(LDLIBS) Libraries/libdaisysp.a Libraries/libowlprg.a

compile: $(BUILD)/$(TARGET).bin
