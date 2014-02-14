#
#	 Copyright (c) 2013 Giuseppe Monteleone, All rights reserved.
#	 contact info@ing-monteleone.com
#
#	 Copyright (C)2014 Marco Cavallini, <m.cavallini@koansoftware.com>
#	 Added linux HOST build support
#
#	 This file is part of 'YasminOS'
#
#	 GNU Lesser General Public License Usage
#	 this file may be used under the terms of the GNU Lesser
#	 General Public License version 2.1 as published by the Free Software
#	 Foundation and appearing in the file LICENSE.LGPL included in the
#	 packaging of this file.  Please review the following information to
#	 ensure the GNU Lesser General Public License version 2.1 requirements
#	 will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
#	 You should have received a copy of the GNU General Public License
#	 along with 'YasminOS'. If not, see <http://www.gnu.org/licenses/>.
#

include config.mk
include source.mk

TARGET = yasminos

# add .. to source and obj directory (are outside build directory)
OBJDIR := ../$(OBJDIR)
SRCDIR := ../$(SRCDIR)

# for now cortex-m0plus is just and alias gcc compiler 4.7.2 doesn't support it 
ifeq ($(CPU),cortex-m0plus)
CPU     := cortex-m0
endif 

# with these define we add architecture macro for code
ifeq ($(CPU),cortex-m0)
ARCH    := V6M
endif
ifeq ($(CPU),cortex-m0plus)
ARCH    := V6M
endif

#todo support cortex m4
ifeq ($(CPU),cortex-m3)
ARCH    := V7M
endif

#if we have a specific platform we define some module and some include to support
#differnt platform
ifneq ($(CPUFAMILY),)
ifeq ($(LDFILE),)
ifneq ($(CPUNAME),)
LDFILE := platform/ld/$(CPUNAME).ld
else
LDFILE := platform/ld/$(CPUFAMILY).ld
endif #$(CPUNAME)

# add src path. Now src act as custom LDFILE with path so following
# rule doesn't chenge it
LDFILE := $(SRCDIR)/$(LDFILE)

# if ld doesn't exist leave default (we ese ($wildcard a,b) function to check file existence)
# we inclue srcdir in condition
ifeq ($(wildcard $(LDFILE)),)
$(info delete-file)
# delete unexistent file 
LDFILE := 
endif #$(wildcard ...)
endif #$(LDFILE)

SRCS   += platform/platform.c
SRCS   += platform/$(CPUFAMILY).c
DEFINE += __use_platform__
DEFINE += __cpu_$(CPUFAMILY)__ 
endif

# define process: we convert define list in compiler options
DEFINE := $(addprefix -D,$(DEFINE))
DEFINE += $(addprefix -U,$(UNDEFINE))
DEFINE += -D__ARCH_$(ARCH)__

#generate objects with objdir preponed 
OBJECTS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
OUTPUT  := $(OBJDIR)/$(TARGET)

#if linker script is not defined, define default
ifeq ($(LDFILE),)
LDFILE  := default.ld
endif

#add src path if no path in $LDFILE
#note we check for '/' or '/' char with $(findstring a,b) function that return
#     found substring a in string b if exist empty string otherwise
#     se makefile documentation
ifeq ($(findstring /,$(LDFILE))$(findstring /,$(LDFILE)),)
LDFILE  := $(SRCDIR)/$(LDFILE)
endif

# optimize string elaboration
ifeq ($(OPTIMIZE),none)
OPTIMIZE := 0
endif
ifeq ($(OPTIMIZE),size)
OPTIMIZE := s
endif 

OPTIMIZE := $(addprefix -O,$(OPTIMIZE))
 

CFLAGS := $(OPTIMIZE) $(DEFINE) -Wall -std=gnu99 -mcpu=$(CPU) -ffunction-sections -mthumb -g3
LDFLAGS := -nostartfiles -mthumb -mcpu=$(CPU) -lc -Wl,--gc-sections -Wl,-Map,$(OUTPUT).map $(OBJECTS) -T$(LDFILE) 
# include path
IPATH  := -I$(SRCDIR) -I$(SRCDIR)/include

.PHONY: all
all: $(OUTPUT).elf $(OUTPUT).lst size 

.PHONY: clean
clean:
	rm $(OBJECTS)
	rm $(OBJECTS:.o=.o.lst)
	rm $(OBJECTS:.o=.d)
	rm $(OUTPUT).lst
	rm $(OUTPUT).elf
	rm $(OUTPUT).map

.PHONY: size
ifneq "$(CC-SIZE)" ""
size: $(OUTPUT).elf
	$(CC-SIZE) --format=barkley $(OUTPUT).elf
else
size:
endif

$(OUTPUT).lst: $(OUTPUT).elf
	$(CC-OBJDUMP) -h -S $< > $@ 

# link command
$(OUTPUT).elf: $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) -o $(OUTPUT).elf 

# compile command	
# With '-MD' we generate dependencies. Dependencies file are the same *.o file (same name, same path) but with *.d extension
$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(IPATH) -MD -c $< -Wa,-ahls=$@.lst -o $@ 

#	@if not exist $(dir $@) mkdir $(dir $@)
#	$(CC) $(CFLAGS) $(IPATH) -MD -c $< -Wa,-ahls=$@.lst -o $@ 
	

# Include dependencies. Include directive with '-' so don't generate error if some file is missing	
-include $(OBJECTS:.o=.d)


