#******************************************************************************
#
# Makefile - Rules for building the project.
#
# Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#   Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the
#   distribution.
#
#   Neither the name of Texas Instruments Incorporated nor the names of
#   its contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision 10636 of the Stellaris Firmware Development Package.
#
#******************************************************************************

# Defines the part type that this project uses.
PART=LM3S9B92

# The base directory for StellarisWare.
ROOT=../stellarisware

# Include make definitions.
include ${ROOT}/makedefs

# Include paths
IPATH=src
IPATH+=${ROOT}
IPATH+=${ROOT}/inc
IPATH+=${ROOT}/driverlib

# Source paths
VPATH=src

# Default rule
all: ${COMPILER}
all: ${COMPILER}/project.axf

# Rule to clean out all build products
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

# Rule to create target directory.
${COMPILER}:
	@mkdir -p ${COMPILER}

# Rules for building the project
${COMPILER}/project.axf: ${COMPILER}/main.o
${COMPILER}/project.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/project.axf: ${ROOT}/driverlib/${COMPILER}-cm3/libdriver-cm3.a
${COMPILER}/project.axf: project.ld
SCATTERgcc_project=project.ld
ENTRY_project=ResetISR

# Include generated dependency files
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
