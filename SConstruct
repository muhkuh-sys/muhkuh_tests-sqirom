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
	src/header.S
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
SConscript('platform/SConscript')
Import('platform_lib_netx500', 'platform_lib_netx56', 'platform_lib_netx50', 'platform_lib_netx10')


def build_netx56_standalone(tEnvBase, strBuildFolder, strOptionSource):
	tEnv = tEnvBase.Clone()
	tEnv.Replace(LDFILE = 'src/netx56/netx56.ld')
	tEnv.Append(CPPPATH = aCppPath)
	tEnv.Append(CPPDEFINES = [['CFG_DEBUGMSG', '1']])
	tSrc = tEnv.SetBuildPath(os.path.join('targets', strBuildFolder), 'src', sources_common+sources_standalone+strOptionSource)
	tElf = tEnv.Elf(os.path.join('targets', strBuildFolder, 'sqitest.elf'), tSrc + platform_lib_netx56)
	
	return tElf


#----------------------------------------------------------------------------
#
# Build all files.
#
aCppPath = ['src', '#platform/src', '#platform/src/lib']

env_netx56_m = env_netx56_default.Clone()
env_netx56_m.Replace(LDFILE = 'src/netx56/netx56.ld')
env_netx56_m.Append(CPPPATH = aCppPath)
env_netx56_m.Append(CPPDEFINES = [['CFG_DEBUGMSG', '1']])
src_netx56_m = env_netx56_m.SetBuildPath('targets/netx56_muhkuh', 'src', sources_common+sources_muhkuh+'src/options_default.c')
elf_netx56_m = env_netx56_m.Elf('targets/netx56_muhkuh/sqitest.elf', src_netx56_m + platform_lib_netx56)
bin_netx56_m = env_netx56_m.ObjCopy('targets/netx56_muhkuh/sqitest.bin', elf_netx56_m)

env_netx56_s = env_netx56_default.Clone()
env_netx56_s.Replace(LDFILE = 'src/netx56/netx56.ld')
env_netx56_s.Append(CPPPATH = aCppPath)
env_netx56_s.Append(CPPDEFINES = [['CFG_DEBUGMSG', '1']])
src_netx56_s = env_netx56_s.SetBuildPath('targets/netx56_standalone', 'src', sources_common+sources_standalone+'src/options_default.c')
elf_netx56_s = env_netx56_s.Elf('targets/netx56_standalone/sqitest.elf', src_netx56_s + platform_lib_netx56)
bb0_netx56_s = env_netx56_s.BootBlock('targets/netx56_spi_intram.img', elf_netx56_s, BOOTBLOCK_SRC='SPI_GEN_10', BOOTBLOCK_DST='INTRAM')

build_netx56_standalone(env_netx56_default, 'netx56_standalone_MX25L3235E', 'src/options_default_MX25L3235E.c')

