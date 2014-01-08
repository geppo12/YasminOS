#
#	 Copyright (c) 2013 Giuseppe Monteleone, All rights reserved.
#	 contact info@ing-monteleone.com
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
#	 THIS SOFTWARE IS PROVIDED BY GIUSEPPE MONTELEONE ``AS IS'' AND ANY
#	 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#	 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#	 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GIUSEPPE MONTELEONE BE
#	 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
#	 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
#	 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#	 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#	 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#	 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#	 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#	 You should have received a copy of the GNU General Public License
#	 along with 'YasminOS'. If not, see <http://www.gnu.org/licenses/>.
#

include config.mk
include source.mk

TARGET = yasminOs

# add .. to source and obj directory (are outside build directory)
OBJDIR := ..\$(OBJDIR)
SRCDIR := ..\$(SRCDIR)

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
LDFILE := platform\ld\$(CPUNAME).ld
else
LDFILE := platform\ld\$(CPUFAMILY).ld
endif #$(CPUNAME)

# add src path. Now src act as custom LDFILE with path so following
# rule doesn't chenge it
LDFILE := $(SRCDIR)\$(LDFILE)

# if ld doesn't exist leave default (we ese ($wildcard a,b) function to check file existence)
# we inclue srcdir in condition
ifeq ($(wildcard $(LDFILE)),)
$(info delete-file)
# delete unexistent file 
LDFILE := 
endif #$(wildcard ...)
endif #$(LDFILE)

SRCS   += platform\platform.c
SRCS   += platform\$(CPUFAMILY).c
DEFINE += __use_platform__
DEFINE += __cpu_$(CPUFAMILY)__ 
endif

# define process: we convert define list in compiler options
DEFINE := $(addprefix -D,$(DEFINE))
DEFINE += $(addprefix -U,$(UNDEFINE))
DEFINE += -D__ARCH_$(ARCH)__

#generate objects with objdir preponed 
OBJECTS := $(addprefix $(OBJDIR)\,$(SRCS:.c=.o))
OUTPUT  := $(OBJDIR)\$(TARGET)

#if linker script is not defined, define default
ifeq ($(LDFILE),)
LDFILE  := default.ld
endif

#add src path if no path in $LDFILE
#note we check for '/' or '\' char with $(findstring a,b) function that return
#     found substring a in string b if exist empty string otherwise
#     se makefile documentation
ifeq ($(findstring \,$(LDFILE))$(findstring /,$(LDFILE)),)
LDFILE  := $(SRCDIR)\$(LDFILE)
endif

ifeq ($(OPTIMIZE),none)
OPTIMIZE := -O0
endif 

CFLAGS := $(OPTIMIZE) $(DEFINE) -Wall -std=gnu99 -mcpu=$(CPU) -ffunction-sections -mthumb -g3
LDFLAGS := -nostartfiles -mthumb -mcpu=$(CPU) -lc -Wl,--gc-sections -Wl,-Map,$(OUTPUT).map $(OBJECTS) -T$(LDFILE) 
# include path
IPATH  := -I$(SRCDIR) -I$(SRCDIR)\include

.PHONY: all
all: $(OUTPUT).elf $(OUTPUT).lst size 

.PHONY: clean
clean:
	del $(OBJECTS) 2>nul
	del $(OBJECTS:.o=.o.lst) 2>nul
	del $(OBJECTS:.o=.d) 2>nul
	del $(OUTPUT).elf 2>nul
	del $(OUTPUT).map 2>nul

.PHONY: size
ifneq "$(CC-SIZE)" ""
size: $(OUTPUT).elf
	$(CC-SIZE) --format=barkley $(OUTPUT).elf
else
size:
endif

$(OUTPUT).lst: $(OUTPUT).elf
	$(CC-OBJDUMP) -h -S $< > $@ 

# link comand
$(OUTPUT).elf: $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) -o $(OUTPUT).elf 

# compile comand	
# With '-MD' we generate dependancies. Dependancies file are the same *.o file (same name, same path) but with *.d extension
$(OBJECTS): $(OBJDIR)\\%.o: $(SRCDIR)\\%.c
	@if not exist $(dir $@) mkdir $(dir $@)
	$(CC) $(CFLAGS) $(IPATH) -MD -c $< -Wa,-ahls=$@.lst -o $@ 
	

# Include depenencies. Include directive with '-' so don't generate error if some file is missing	
-include $(OBJECTS:.o=.d)

	
