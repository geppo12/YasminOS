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

PREFIX := arm-poky-linux-gnueabi-

# c compiler mandatory
CC         := ${PREFIX}gcc
# dump utility mandatory
CC-OBJDUMP := ${PREFIX}objdump

# size utility: if your compiler haven't size command, comment following line 
CC-SIZE    := ${PREFIX}size

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

CPUFAMILY  := mb9bf500
CPUNAME    :=

# custom linker script. Leave empty for default 
LDFILE   := 

# option define 
DEFINE   := USE_DISABLE_ISP
DEFINE   += USE_ASSERT
DEFINE	 += USE_IDLE_TASK
#DEFINE   += USE_PRINTF

SRCDIR   := src
OBJDIR   := sbjects

