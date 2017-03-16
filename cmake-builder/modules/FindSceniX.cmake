# Copyright NVIDIA Corporation 2009
# TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
# *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
# OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
# BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
# WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
# BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
# ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
# BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

# If SceniX home is not set, we cannot find SceniX
if("$ENV{SCENIXHOME}" STREQUAL "" )
	# Die hard, if SceniX was required, otherwise only warn
	if(SceniX_FIND_REQUIRED)
		MESSAGE(SEND_ERROR "SceniX NOT FOUND ($SCENIXHOME==''), is it properly installed?") 
	else(SceniX_FIND_REQUIRED)
		MESSAGE("SceniX NOT FOUND ($SCENIXHOME==''), is it properly installed?") 
	endif(SceniX_FIND_REQUIRED)
	return()
endif( "$ENV{SCENIXHOME}" STREQUAL "" )

if ( "${SCENIX_HOME}" STREQUAL "" )
  file(TO_CMAKE_PATH $ENV{SCENIXHOME} SCENIX_HOME_TMP)
  set( SCENIX_HOME ${SCENIX_HOME_TMP} CACHE PATH "SceniX SDK directory" )
endif()

###############################################################################
# Set some system specific vars
if(UNIX) 
    set(OS "linux")
else(UNIX)
    if(APPLE)
    else(APPLE)
        if(WIN32)
    set(OS "win")
        endif(WIN32)
    endif(APPLE)
endif(UNIX)

###############################################################################
# Detect pieces of the bin/lib path
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set ( ARCH "amd64" )
else ()
  set ( ARCH "x86" )
endif()

if (WIN32)
  add_definitions("/wd4996")
endif(WIN32)

set(HAVE_NULLPTR 0)

if(MSVC90)
  set (CRT "crt90")
elseif(MSVC10)
  set (CRT "crt100")
  set (HAVE_NULLPTR 1)
endif(MSVC90)

set( SCENIX_CRT  ${CRT}  CACHE STRING "SceniX runtime used by compiler" )
set( SCENIX_ARCH ${ARCH} CACHE STRING "Compiler architecture" )

if ( UNIX )
  set( SCENIX_3RDPARTY_PATH "$ENV{SCENIXHOME}/../../buildtools/Linux" CACHE STRING "SceniX 3rd party libraries & compilers" )
elseif ( WIN32 )
  if ( "$ENV{SCENIX_3RDPARTY_PATH}" STREQUAL "")
    set( SCENIX_3RDPARTY_PATH "$ENV{SCENIXHOME}\\..\\..\\buildtools\\win32" CACHE STRING "SceniX 3rd party libraries & compilers" )
  else()
    set( SCENIX_3RDPARTY_PATH $ENV{SCENIX_3RDPARTY_PATH} )
  endif()
endif(UNIX)

set( ENV{SCENIX_3RDPARTY_PATH} ${SCENIX_3RDPARTY_PATH} )

###############################################################################
# Set compiler flags and compiler definitions

if( NOT HAVE_NULLPTR )
  add_definitions("-Dnullptr=0")
endif()

if ("${CMAKE_GENERATOR}" MATCHES "^(Visual Studio).*")
  set (SCENIX_CONFIG "$(ConfigurationName)")
else()
  if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
    set (SCENIX_CONFIG "release")
  else()
    string(TOLOWER ${CMAKE_BUILD_TYPE} SCENIX_CONFIG )
  endif()
endif()

if (UNIX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffriend-injection")
  add_definitions("-DLINUX")
  if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    add_definitions("-DNDEBUG")
  endif()
  
  set( SCENIX_BINARY_DIR "${SCENIX_HOME}/bin/${ARCH}/${OS}/${SCENIX_CONFIG}" )
elseif(WIN32)
  add_definitions("-DNOMINMAX")
  set( SCENIX_BINARY_DIR "${SCENIX_HOME}/bin/${ARCH}/${OS}/${CRT}/${SCENIX_CONFIG}" )
endif()

###############################################################################
# Determine SceniX Version number

file(STRINGS ${SCENIX_HOME}/inc/nvsg/nvsg/NVSGVersion.h NVSGVERSION)
foreach(LINE ${NVSGVERSION})
  if("${LINE}" MATCHES ".*\#define NVSG_VER_MAJOR[ ]+([0-9]+).*")
    string(REGEX REPLACE ".*\#define NVSG_VER_MAJOR[ ]+([0-9]+).*" "\\1" SCENIX_VER_MAJOR "${LINE}")
  endif()
  if("${LINE}" MATCHES ".*\#define NVSG_VER_MINOR[ ]+([0-9]+).*")
    string(REGEX REPLACE ".*\#define NVSG_VER_MINOR[ ]+([0-9]+).*" "\\1" SCENIX_VER_MINOR "${LINE}")
  endif()
endforeach()

###############################################################################
# SceniX include direcotries

set (SCENIX_INCLUDES
  "${SCENIX_HOME}/inc"
  "${SCENIX_HOME}/inc/nvsg"
  "${SCENIX_HOME}/inc/nvsg/nvgl"
)

set (SCENIXRT_INCLUDES
  "${SCENIX_HOME}/inc/nvrt"
  "${SCENIX_HOME}/inc/nvrt/nvirt"
  "${SCENIX_HOME}/inc/nvrt/nvirt/nvirtu"
)

set (SCENIXNEURAY_INCLUDES
  "${SCENIX_HOME}/inc/nvneuray"
  "${SCENIX_HOME}/inc/nvneuray/neuray"
)

###############################################################################
# SceniX libraries

if (WIN32)
	set (SCENIX_LIBS
	    debug "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/SceniX${SCENIX_VER_MAJOR}.lib"
	    optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/SceniX${SCENIX_VER_MAJOR}.lib"
	)

	set (SCENIXRT_LIBS
	    debug "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/SceniXRT${SCENIX_VER_MAJOR}.lib"
	    optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/SceniXRT${SCENIX_VER_MAJOR}.lib"
	)
  
	set (SCENIXNEURAY_LIBS
	    debug "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/SceniXNeuray${SCENIX_VER_MAJOR}.lib"
	    optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/SceniXNeuray${SCENIX_VER_MAJOR}.lib"
	)
	
  set (SCENIXCG_LIBS
       debug     "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/cg.lib"   
       debug     "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/cgGL.lib"
       optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/cg.lib"
       optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/cgGL.lib"
  )

  set (SCENIXRAMFC_LIBS
    debug     "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/debug/RAmfc.lib"   
    optimized "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/release/RAmfc.lib"
  )


elseif(UNIX)
	set (SCENIX_LIBS
	    debug "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libscenix.so.${SCENIX_VER_MAJOR}.${SCENIX_VER_MINOR}"
	    debug "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libCg.so"
	    debug "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libCgGL.so"
	    optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libscenix.so.${SCENIX_VER_MAJOR}.${SCENIX_VER_MINOR}"
	    optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libCg.so"
	    optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libCgGL.so"
	)

	set (SCENIXRT_LIBS
	    debug "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libscenixrt.so.${SCENIX_VER_MAJOR}.${SCENIX_VER_MINOR}"
	    optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libscenixrt.so.${SCENIX_VER_MAJOR}.${SCENIX_VER_MINOR}"
	)

	set (SCENIXNEURAY_LIBS
	    debug     "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libSceniXneuray${SCENIX_VER_MAJOR}.so"
	    optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libSceniXneuray${SCENIX_VER_MAJOR}.so"
	)
  
  set (SCENIXCG_LIBS
       debug     "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libCg.so"   
       debug     "${SCENIX_HOME}/bin/${ARCH}/${OS}/debug/libCgGL.so"
       optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libCg.so"
       optimized "${SCENIX_HOME}/bin/${ARCH}/${OS}/release/libCgGL.so"
      )  
endif()

macro(SCENIX_SET_BINARY_FOLDERS)
  if(WIN32)
    if ("${CMAKE_GENERATOR}" MATCHES "^(Visual Studio).*")
      set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}" )
      set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${CRT}" )
      set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${CRT}" )
    else()
      set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${SCENIX_HOME}/lib/${ARCH}/${OS}/${CRT}/${SCENIX_CONFIG}" )
      set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${CRT}/${SCENIX_CONFIG}" )
      set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${CRT}/${SCENIX_CONFIG}" )
    endif()
  elseif(UNIX)
      set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${SCENIX_HOME}/lib/${ARCH}/${OS}/${SCENIX_CONFIG}" )
      set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${SCENIX_CONFIG}" )
      set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SCENIX_HOME}/bin/${ARCH}/${OS}/${SCENIX_CONFIG}" )
  endif()
endmacro()

