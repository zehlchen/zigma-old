#
# ZIGMA, Copyright (C) 1999, 2005, 2023 Chase Zehl O'Byrne
#   <mail: zehl@live.com> http://zehlchen.com/
#
# This file is part of ZIGMA.
#
# ZIGMA is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# ZIGMA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ZIGMA; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

cmake_minimum_required(VERSION 3.16)

project (zigma
  VERSION 1.2.1
  DESCRIPTION "ZIGMA"
)

execute_process(COMMAND git rev-list --count HEAD
  OUTPUT_VARIABLE GIT_BUILD
  ERROR_QUIET
)

string(REGEX REPLACE "\n$" "" GIT_BUILD "${GIT_BUILD}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(zigma)
target_sources(zigma PRIVATE
  zigma/base64.c
  zigma/driver.c
  zigma/kvlist.c
  zigma/zigma.c
)
