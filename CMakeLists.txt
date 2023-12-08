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
  VERSION 1.2.2
  DESCRIPTION "ZIGMA is a command line utility for encrypting, decrypting, and signing files."
)

execute_process(
  COMMAND git rev-list --count HEAD
  OUTPUT_VARIABLE GIT_BUILD
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git describe --tags --abbrev=0
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
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

add_compile_definitions(
  GIT_BUILD="${GIT_BUILD}"
  GIT_COMMIT="${GIT_COMMIT}"
  GIT_BRANCH="${GIT_BRANCH}"
  GIT_TAG="${GIT_TAG}"
  ZIGMA_VERSION_STRING="${PROJECT_VERSION}:${GIT_BUILD}"
)
