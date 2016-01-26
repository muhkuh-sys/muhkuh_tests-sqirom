# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------#
#   Copyright (C) 2012 by Christoph Thelen                                #
#   doc_bacardi@users.sourceforge.net                                     #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
#-------------------------------------------------------------------------#


import os.path

#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('env_default')


#----------------------------------------------------------------------------
# This is the list of sources. The elements must be separated with whitespace
# (i.e. spaces, tabs, newlines). The amount of whitespace does not matter.
sources_common = """
	src/boot_drv_sqi.c
	src/boot_spi.c
	src/boot_sqi_xip.c
	src/header.c
"""

sources_muhkuh = """
	src/init_netx_test.S
	src/main.c
	src/netx_test.c
"""

sources_standalone = """
	src/init_standalone.S
	src/main_standalone.c
"""

#----------------------------------------------------------------------------
#
# Create the compiler environments.
#
env_netx500_default = env_default.CreateCompilerEnv('500', ['cpu=arm926ej-s'])
env_netx500_default.Replace(BOOTBLOCK_CHIPTYPE = 500)

env_netx56_default = env_default.CreateCompilerEnv('56', ['cpu=arm966e-s'])
env_netx56_default.Replace(BOOTBLOCK_CHIPTYPE = 56)

env_netx50_default = env_default.CreateCompilerEnv('50', ['cpu=arm966e-s'])
env_netx50_default.Replace(BOOTBLOCK_CHIPTYPE = 50)

env_netx10_default = env_default.CreateCompilerEnv('10', ['cpu=arm966e-s'])
env_netx10_default.Replace(BOOTBLOCK_CHIPTYPE = 10)

Export('env_netx500_default', 'env_netx56_default', 'env_netx50_default', 'env_netx10_default')


#----------------------------------------------------------------------------
#
# Build the platform library.
#
PLATFORM_LIB_CFG_BUILDS = [500, 56, 50, 10]
SConscript('platform/SConscript', exports='PLATFORM_LIB_CFG_BUILDS')
Import('platform_lib_netx500', 'platform_lib_netx56', 'platform_lib_netx50', 'platform_lib_netx10')



#----------------------------------------------------------------------------
#
# Get the source code version from the VCS.
#
env_default.Version('targets/version/version.h', 'templates/version.h')



#----------------------------------------------------------------------------
#
# This is a helper function which generates a sequence of pseudo random
# numbers.
#
def prn_obj(tEnv, sizSequence, strPrnBinFilename):
	# Convert the size into a "long" to prevent floats.
	ulSequenceSize = long(sizSequence)
	
	# Generate the sequence.
	tPrnBin = tEnv.Prn(strPrnBinFilename, None, PRN_SIZE=ulSequenceSize)
	
	# Convert the binary file into an object.
	strLabelPath = strPrnBinFilename.replace('/', '_').replace('.', '_').replace('\\', '_')
	strOutput = os.path.splitext(strPrnBinFilename)[0] + tEnv['OBJSUFFIX']
	strCmd = '"$OBJCOPY" -v -I binary -O elf32-littlearm -B ARM --rename-section .data=.prn_rodata --redefine-sym "_binary_%s_start"="_binary_test_bin_start" --redefine-sym "_binary_%s_end"="_binary_test_bin_end" $SOURCE $TARGET' % (strLabelPath, strLabelPath)
	return tEnv.Command(strOutput, tPrnBin, strCmd)


#----------------------------------------------------------------------------

def build_netx56_muhkuh(tEnvBase, strBuildId, strOptionSource, tPlatformLib):
	tEnv = tEnvBase.Clone()
	tSrc = tEnv.SetBuildPath(os.path.join('targets', strBuildId), 'src', sources_common+sources_muhkuh+strOptionSource)
	tElf = tEnv.Elf(os.path.join('targets', strBuildId, 'sqitest.elf'), tSrc + tPlatformLib)
	tBin = tEnv.ObjCopy(os.path.join('targets', strBuildId+'.bin'), tElf)
	
	return tBin



def build_netx56_standalone(tEnvBase, strBuildFolder, strOptionSource, tPlatformLib):
	tEnv = tEnvBase.Clone()
	tSrc = tEnv.SetBuildPath(os.path.join('targets', strBuildFolder), 'src', sources_common+sources_standalone+strOptionSource)
	tPrn = prn_obj(tEnv, 0x00008000, os.path.join('targets', strBuildFolder, 'prn_128k.bin'))
	tElf = tEnv.Elf(os.path.join('targets', strBuildFolder, 'sqitest.elf'), tSrc + tPlatformLib + tPrn)
	
	return tElf


#----------------------------------------------------------------------------
#
# Build all files.
#
aCppPath = ['src', '#platform/src', '#platform/src/lib', '#targets/version']

tEnv_netx56 = env_netx56_default.Clone()
tEnv_netx56.Replace(LDFILE = 'src/netx56/netx56.ld')
tEnv_netx56.Append(CPPPATH = aCppPath)
tEnv_netx56.Append(CPPDEFINES = [['CFG_DEBUGMSG', '1']])


tBin0 = build_netx56_muhkuh(tEnv_netx56, 'sqirom_test_netx56_Winbond_W25Q32',      'src/options_default_W25Q32.c',     platform_lib_netx56)
tBin1 = build_netx56_muhkuh(tEnv_netx56, 'sqirom_test_netx56_Micron_N25Q032A',     'src/options_default_N25Q032A.c',   platform_lib_netx56)
tBin2 = build_netx56_muhkuh(tEnv_netx56, 'sqirom_test_netx56_Macronix_MX25L3235E', 'src/options_default_MX25L3235E.c', platform_lib_netx56)
tBin3 = build_netx56_muhkuh(tEnv_netx56, 'sqirom_test_netx56_Macronix_MX25L3273F', 'src/options_default_MX25L3273F.c', platform_lib_netx56)


tElf_netx56_W25Q32_s = build_netx56_standalone(tEnv_netx56, 'netx56_standalone_Winbond_W25Q32', 'src/options_default_W25Q32.c', platform_lib_netx56)
bb0_netx56_s = tEnv_netx56.BootBlock('targets/sqirom_test_standalone_netx56_Winbond_W25Q32.img', tElf_netx56_W25Q32_s, BOOTBLOCK_SRC='SPI_GEN_10', BOOTBLOCK_DST='INTRAM')

tElf_netx56_MX25L3235E_s = build_netx56_standalone(tEnv_netx56, 'netx56_standalone_Macronix_MX25L3235E', 'src/options_default_MX25L3235E.c', platform_lib_netx56)
bb1_netx56_s = tEnv_netx56.BootBlock('targets/sqirom_test_standalone_netx56_Macronix_MX25L3235E.img', tElf_netx56_MX25L3235E_s, BOOTBLOCK_SRC='SPI_GEN_10', BOOTBLOCK_DST='INTRAM')

tElf_netx56_N25Q032A_s = build_netx56_standalone(tEnv_netx56, 'netx56_standalone_Micron_N25Q032A', 'src/options_default_N25Q032A.c', platform_lib_netx56)
bb2_netx56_s = tEnv_netx56.BootBlock('targets/sqirom_test_standalone_netx56_Micron_N25Q032A.img', tElf_netx56_N25Q032A_s, BOOTBLOCK_SRC='SPI_GEN_10', BOOTBLOCK_DST='INTRAM')

tElf_netx56_MX25L3273F_s = build_netx56_standalone(tEnv_netx56, 'netx56_standalone_Macronix_MX25L3273F', 'src/options_default_MX25L3273F.c', platform_lib_netx56)
bb3_netx56_s = tEnv_netx56.BootBlock('targets/sqirom_test_standalone_netx56_Macronix_MX25L3273F.img', tElf_netx56_MX25L3273F_s, BOOTBLOCK_SRC='SPI_GEN_10', BOOTBLOCK_DST='INTRAM')


#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binaries.
Command('targets/testbench/netx/sqirom_test_netx56_Winbond_W25Q32.bin',  tBin0, Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/sqirom_test_netx56_Micron_N25Q032A',     tBin1, Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/sqirom_test_netx56_Macronix_MX25L3235E', tBin2, Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/sqirom_test_netx56_Macronix_MX25L3273F', tBin3, Copy("$TARGET", "$SOURCE"))

