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
DEFINE := $(addprefix -D,$(DEFINE))
DEFINE += $(addprefix -U,$(UNDEFINE))
DEFINE += -D__arch_$(subst -,_,$(ARCH))__ 

# not not configurable options
CFLAGS := $(OPTIMIZE) $(DEFINE) -Wall -std=gnu99 -mcpu=$(ARCH) -ffunction-sections -mthumb -g3
# include path
IPATH  := -I$(SRCDIR)\include

#generate objects with objdir preponed 
OBJECTS := $(addprefix $(OBJDIR)\,$(SRCS:.c=.o))
OUTPUT  := $(OBJDIR)\$(TARGET)

.PHONY: all
all: $(OUTPUT).elf size

.PHONY: clean
clean:
	del $(OBJECTS)
	del $(OUTPUT).elf
	del $(OUTPUT).map

.PHONY: size
ifneq "$(CC-SIZE)" ""
size: $(OUTPUT).elf
	$(CC-SIZE) --format=barkley $(OUTPUT).elf
else
size:
endif 

# link comand
$(OUTPUT).elf: $(OBJECTS) $(SRCDIR)\application.ld
	$(CC) -nostartfiles -gc-sections -Wl,-Map,$(OUTPUT).map $(OBJECTS) -T$(SRCDIR)\application.ld -o $(OUTPUT).elf

# compile comand	
# With '-MD' we generate dependancies. Dependancies file are the same *.o file (same name, same path) but with *.d extension
$(OBJECTS): $(OBJDIR)\\%.o: $(SRCDIR)\\%.c
	@if not exist $(dir $@) mkdir $(dir $@)
	$(CC) $(CFLAGS) $(IPATH) -MD -c $< -o $@
	

# Include depenencies. Include directive with '-' so don't generate error if some file is missing	
-include $(OBJECTS:.o=.d)

	
