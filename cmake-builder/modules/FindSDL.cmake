# Locate SDL library
# This module defines
# SDL_LIBRARY, the name of the library to link against
# SDL_FOUND, if false, do not try to link to SDL
# SDL_INCLUDE_DIR, where to find SDL.h
#
# This module responds to the the flag:
# SDL_BUILDING_LIBRARY
# If this is defined, then no SDL_main will be linked in because
# only applications need main().
# Otherwise, it is assumed you are building an application and this
# module will attempt to locate and set the the proper link flags
# as part of the returned SDL_LIBRARY variable.
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the
# OS X framework based version. (Other versions link to -lSDLmain which
# this module will try to find on your behalf.) Also for OS X, this
# module will automatically add the -framework Cocoa on your behalf.
#
#
# Additional Note: If you see an empty SDL_LIBRARY in your configuration
# and no SDL_LIBRARY, it means CMake did not find your SDL library
# (SDL.dll, libsdl.so, SDL.framework, etc).
# Set SDL_LIBRARY to point to your SDL library, and configure again.
# Similarly, if you see an empty SDL_MAIN_LIBRARY, you should set this value
# as appropriate. These values are used to generate the final SDL_LIBRARY
# variable, but when these values are unset, SDL_LIBRARY does not get created.
#
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
# l.e.galup  9-20-02
#
# Modified by Eric Wing.
# Added code to assist with automated building by using environmental variables
# and providing a more controlled/consistent search behavior.
# Added new modifications to recognize OS X frameworks and
# additional Unix paths (FreeBSD, etc).
# Also corrected the header search path to follow "proper" SDL guidelines.
# Added a search for SDLmain which is needed by some platforms.
# Added a search for threads which is needed by some platforms.
# Added needed compile switches for MinGW.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of
# SDL_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from SDL/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention
# is #include "SDL.h", not <SDL/SDL.h>. This is done for portability
# reasons because not all systems place things in SDL/ (see FreeBSD).

FIND_PATH(SDL_INCLUDE_DIR SDL.h
  HINTS
  $ENV{SDLDIR}
  PATH_SUFFIXES include/SDL include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  ${SDL_ROOT}
  ../contrib
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt  
)
#MESSAGE("SDL_INCLUDE_DIR is ${SDL_INCLUDE_DIR}")

# SDL-1.1 is the name used by FreeBSD ports...
# don't confuse it for the version number.
FIND_LIBRARY(SDL_LIBRARY
  NAMES SDL SDL-1.1    
  PATH_SUFFIXES lib64 lib x86_64-linux-gnu
  PATHS
  ${SDL_ROOT}/lib/${ARCH}
  ../contrib/lib/${ARCH}
  /sw
  /opt/local
  /opt/csw
  /opt 
)

FIND_LIBRARY(SDL_LIBRARY_DEBUG
  NAMES SDLd  
  PATH_SUFFIXES lib64 lib x86_64-linux-gnu
  PATHS
  ${SDL_ROOT}/lib/${ARCH}
  ../contrib/lib/${ARCH}
  /sw
  /opt/local
  /opt/csw
  /opt
)

#MESSAGE("SDL_LIBRARY is ${SDL_LIBRARY}")

IF(NOT SDL_BUILDING_LIBRARY)
  IF(NOT ${SDL_INCLUDE_DIR} MATCHES ".framework")
    # Non-OS X framework versions expect you to also dynamically link to
    # SDLmain. This is mainly for Windows and OS X. Other (Unix) platforms
    # seem to provide SDLmain for compatibility even though they don't
    # necessarily need it.
    FIND_LIBRARY(SDL_MAIN_LIBRARY
      NAMES SDLmain SDLmain-1.1
      HINTS
      $ENV{SDLDIR}
      PATH_SUFFIXES lib64 lib
      PATHS
      ${SDL_ROOT}/lib/${ARCH}
      ../contrib/lib/${ARCH}
      /sw
      /opt/local
      /opt/csw
      /opt
    )
    FIND_LIBRARY(SDL_MAIN_LIBRARY_DEBUG
      NAMES SDLmaind
      HINTS
      $ENV{SDLDIR}
      PATH_SUFFIXES lib64 lib
      PATHS
      ${SDL_ROOT}/lib/${ARCH}
      ../contrib/lib/${ARCH}
      /sw
      /opt/local
      /opt/csw
      /opt
    )
  ENDIF(NOT ${SDL_INCLUDE_DIR} MATCHES ".framework")
ENDIF(NOT SDL_BUILDING_LIBRARY)

# SDL may require threads on your system.
# The Apple build may not need an explicit flag because one of the
# frameworks may already provide it.
# But for non-OSX systems, I will use the CMake Threads package.
IF(NOT APPLE)
  FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDLmain -lSDL -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
IF(MINGW)
  SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

SET(SDL_FOUND "NO")
SET(SDL_LIBRARIES "")
IF(SDL_LIBRARY AND SDL_INCLUDE_DIR)
  GET_FILENAME_COMPONENT( SDL_LIBRARY_NAME ${SDL_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( SDL_LIBRARY_PATH ${SDL_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( SDL_MAIN_LIBRARY_NAME ${SDL_MAIN_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( SDL_MAIN_LIBRARY_PATH ${SDL_MAIN_LIBRARY} PATH )
  # For OS X, SDL uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the
  # "real" variable in one-shot.
  IF(APPLE)
    SET(SDL_LIBRARIES ${SDL_LIBRARIES} "-framework Cocoa")
  ENDIF(APPLE)

  # For threads, as mentioned Apple doesn't need this.
  # In fact, there seems to be a problem if I used the Threads package
  # and try using this line, so I'm just skipping it entirely for OS X.
  IF(NOT APPLE)
    SET(SDL_LIBRARIES ${SDL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  ENDIF(NOT APPLE)

  # For MinGW library
  IF(MINGW)
    SET(SDL_LIBRARIES ${SDL_LIBRARIES} ${MINGW32_LIBRARY})
  ENDIF(MINGW)

  # Set the final string here so the GUI reflects the final state.
  IF(WIN32)
    SET(SDL_LIBRARIES optimized ${SDL_LIBRARY_NAME})
    IF(SDL_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( SDL_LIBRARY_NAME_DEBUG ${SDL_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( SDL_LIBRARY_PATH_DEBUG ${SDL_LIBRARY_DEBUG} PATH )
      SET(SDL_LIBRARIES ${SDL_LIBRARIES} debug ${SDL_LIBRARY_NAME_DEBUG})
    ENDIF(SDL_LIBRARY_DEBUG)

    IF(NOT SDL_BUILDING_LIBRARY)
      IF(SDL_MAIN_LIBRARY)
        SET(SDL_LIBRARIES ${SDL_LIBRARIES} optimized ${SDL_MAIN_LIBRARY_NAME})
      ENDIF(SDL_MAIN_LIBRARY)
      IF(SDL_MAIN_LIBRARY_DEBUG)
        GET_FILENAME_COMPONENT( SDL_MAIN_LIBRARY_NAME_DEBUG ${SDL_MAIN_LIBRARY_DEBUG} NAME )
        GET_FILENAME_COMPONENT( SDL_MAIN_LIBRARY_PATH_DEBUG ${SDL_MAIN_LIBRARY_DEBUG} PATH )
        SET(SDL_LIBRARIES ${SDL_LIBRARIES} debug ${SDL_MAIN_LIBRARY_NAME_DEBUG})
      ENDIF(SDL_MAIN_LIBRARY_DEBUG)
    ENDIF(NOT SDL_BUILDING_LIBRARY)
  ELSE(WIN32)
    SET(SDL_LIBRARIES general ${SDL_LIBRARY_NAME})
    IF(NOT SDL_BUILDING_LIBRARY)
      IF(SDL_MAIN_LIBRARY)
        SET(SDL_LIBRARIES ${SDL_LIBRARIES} general ${SDL_MAIN_LIBRARY_NAME})
      ENDIF(SDL_MAIN_LIBRARY)
    ENDIF(NOT SDL_BUILDING_LIBRARY)
  ENDIF(WIN32)

  # Set the final list of paths and remove duplicates
  SET(SDL_LIBRARY_DIRS ${SDL_LIBRARY_PATH} ${SDL_LIBRARY_PATH_DEBUG})
  IF(NOT SDL_BUILDING_LIBRARY)
    LIST(APPEND SDL_LIBRARY_DIRS ${SDL_MAIN_LIBRARY_PATH} ${SDL_MAIN_LIBRARY_PATH_DEBUG})
  ENDIF(NOT SDL_BUILDING_LIBRARY)

  SET(SDL_LIBRARIES ${SDL_LIBRARIES} CACHE STRING "List of SDL libraries")

  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(SDL_LIBRARY "${SDL_LIBRARY}" CACHE INTERNAL "")
  SET(SDL_MAIN_LIBRARY "${SDL_MAIN_LIBRARY}" CACHE INTERNAL "")
  SET(SDL_LIBRARY_DEBUG "${SDL_LIBRARY_DEBUG}" CACHE INTERNAL "")
  SET(SDL_MAIN_LIBRARY_DEBUG "${SDL_MAIN_LIBRARY_DEBUG}" CACHE INTERNAL "")

  SET(SDL_FOUND "YES")
ENDIF(SDL_LIBRARY AND SDL_INCLUDE_DIR)

#MESSAGE("SDL_LIBRARY is ${SDL_LIBRARY}")

IF(${SDL_FOUND} STREQUAL "NO")
  MESSAGE("SDL library not found.")
ENDIF(${SDL_FOUND} STREQUAL "NO")

