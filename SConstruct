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


#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('env_default')


#----------------------------------------------------------------------------
# This is the list of sources. The elements must be separated with whitespace
# (i.e. spaces, tabs, newlines). The amount of whitespace does not matter.
sources = """
	src/header.S
	src/burst_access.s
	src/init_netx_test.S
	src/main.c
	src/netx_test.c
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


#----------------------------------------------------------------------------
#
# Build all files.
#
aCppPath = ['src', '#platform/src', '#platform/src/lib']

env_netx500 = env_netx500_default.Clone()
env_netx500.Replace(LDFILE = 'src/netx500/netx500.ld')
env_netx500.Append(CPPPATH = aCppPath)
src_netx500 = env_netx500.SetBuildPath('targets/netx500', 'src', sources)
elf_netx500 = env_netx500.Elf('targets/netx500/ramtest.elf', src_netx500 + platform_lib_netx500)
bin_netx500 = env_netx500.ObjCopy('targets/netx500/ramtest.bin', elf_netx500)

env_netx56 = env_netx56_default.Clone()
env_netx56.Replace(LDFILE = 'src/netx56/netx56.ld')
env_netx56.Append(CPPPATH = aCppPath)
src_netx56 = env_netx56.SetBuildPath('targets/netx56', 'src', sources)
elf_netx56 = env_netx56.Elf('targets/netx56/ramtest.elf', src_netx56 + platform_lib_netx56)
bin_netx56 = env_netx56.ObjCopy('targets/netx56/ramtest.bin', elf_netx56)

env_netx50 = env_netx50_default.Clone()
env_netx50.Replace(LDFILE = 'src/netx50/netx50.ld')
env_netx50.Append(CPPPATH = aCppPath)
src_netx50 = env_netx50.SetBuildPath('targets/netx50', 'src', sources)
elf_netx50 = env_netx50.Elf('targets/netx50/ramtest.elf', src_netx50 + platform_lib_netx50)
bin_netx50 = env_netx50.ObjCopy('targets/netx50/ramtest.bin', elf_netx50)

env_netx10 = env_netx10_default.Clone()
env_netx10.Replace(LDFILE = 'src/netx10/netx10.ld')
env_netx10.Append(CPPPATH = aCppPath)
src_netx10 = env_netx10.SetBuildPath('targets/netx10', 'src', sources)
elf_netx10 = env_netx10.Elf('targets/netx10/ramtest.elf', src_netx10 + platform_lib_netx10)
bin_netx10 = env_netx10.ObjCopy('targets/netx10/ramtest.bin', elf_netx10)


#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all public binaries.
Command('targets/testbench/netx/ramtest_netx10.bin',  bin_netx10,  Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/ramtest_netx50.bin',  bin_netx50,  Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/ramtest_netx56.bin',  bin_netx56,  Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/netx/ramtest_netx500.bin', bin_netx500, Copy("$TARGET", "$SOURCE"))

# Copy all LUA scripts.
Command('targets/testbench/ramtest_IS42S32800B.lua',  'lua/ramtest_IS42S32800B.lua', Copy("$TARGET", "$SOURCE"))
Command('targets/testbench/ramtest_MT48LC2M32.lua',   'lua/ramtest_MT48LC2M32.lua',  Copy("$TARGET", "$SOURCE"))
