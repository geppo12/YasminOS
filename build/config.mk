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
#	 EXAMPLE EXCEPTION
#	 This is and example file. Modification of this file does not constitute
#	 a derivative work of YasminOS
#

# c compiler mandatory
CC         := arm-none-eabi-gcc
# dump utility mandatory
CC-OBJDUMP := arm-none-eabi-objdump

# size utility: if your compiler haven't size command, comment following line 
CC-SIZE    := arm-none-eabi-size

# optimization
# supported value:
#
# none
# 1
# 2
# 3
# size
OPTIMIZE   := none

#########################
# TARGET DEFINE
#########################
# supported core 
#
# cortex-m0
# cortex-m0plus	(for now alis for cortex-m0)
# cortex-m3
 
CPU      := cortex-m3

#########################
# PLATFORM FEATURE ENABLE
#########################
# we have two macro: 
# CPUNAME = exact cpu name
# CPUFAMILY = family of cpu form CPUNAME derive
# use these macro activate platform feature
# To use thsi feature CPUFAMILY is mandatory and 
# CPUNAME is optional   
#
# SUPPORTED FAMILY 
# mb9bf500
#
# SUPPORTED NAME
# none, leave blank

CPUFAMILY  :=  
CPUNAME    :=

# custom linker script. Leave empty for default
# use "ld::" prefix for use ld default directory 
LDFILE   := ld::lpc40xx.ld

# option define 
#DEFINE   := USE_DISABLE_ISP
DEFINE   += USE_ASSERT
DEFINE	 += USE_IDLE_TASK
#DEFINE   += USE_PRINTF

SRCDIR   := src
OBJDIR   := objects

