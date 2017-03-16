# - Try to find GLEW
# Once done this will define
#
#  GLEW_ROOT          - root directory for searching process
#
#  GLEW_FOUND         - system has GLEW
#  GLEW_INCLUDE_DIR   - the GLEW include directory
#  GLEW_LIBRARY_DIR   - where the libraries are
#  GLEW_LIBRARY       - Link these to use GLEW
#  GLEW_LIBRARY_DEBUG - Link these to use GLEW
#  GLEW_LIBRARIES
#

IF (GLEW_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GLEW_FIND_QUIETLY TRUE)
ENDIF (GLEW_INCLUDE_DIR)

if( WIN32 )
   FIND_PATH( GLEW_INCLUDE_DIR gl/glew.h gl/wglew.h
              PATHS ${GLEW_ROOT}/include ../contrib/include c:/glew/include )
   FIND_LIBRARY( GLEW_LIBRARY
                 NAMES glew32 glew64
                 PATHS ${GLEW_ROOT}/lib/${ARCH} ../contrib/lib/${ARCH} c:/glew/lib  ${GLEW_ROOT})
   FIND_LIBRARY( GLEW_LIBRARY_DEBUG
                 NAMES glew32d glew64d
                 PATHS ${GLEW_ROOT}/lib/${ARCH} ../contrib/lib/${ARCH} c:/glew/lib ${GLEW_ROOT})
else( WIN32 )
   FIND_PATH( GLEW_INCLUDE_DIR glew.h wglew.h
              PATHS /usr/local/include /usr/include
              PATH_SUFFIXES gl/ GL/)
   FIND_LIBRARY( GLEW_LIBRARY
                 NAMES glew GLEW
                 PATHS /usr/lib /usr/local/lib)
endif( WIN32 )

IF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
  SET( GLEW_FOUND TRUE )
  GET_FILENAME_COMPONENT( GLEW_LIBRARY_DIR ${GLEW_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( GLEW_LIBRARY ${GLEW_LIBRARY} NAME )

  IF(WIN32)
    SET(GLEW_LIBRARIES optimized ${GLEW_LIBRARY})
    IF(GLEW_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( GLEW_LIBRARY_DEBUG ${GLEW_LIBRARY_DEBUG} NAME )
      SET(GLEW_LIBRARIES ${GLEW_LIBRARIES} debug ${GLEW_LIBRARY_DEBUG})
    ENDIF(GLEW_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET( GLEW_LIBRARIES general ${GLEW_LIBRARY})
  ENDIF(WIN32)

ELSE (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
   SET( GLEW_FOUND FALSE )
   SET( GLEW_LIBRARY_DIR )
   SET( GLEW_LIBRARIES )
ENDIF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)

SET(GLEW_LIBRARIES ${GLEW_LIBRARIES} CACHE STRING "List of GLEW libraries")

MARK_AS_ADVANCED(
  GLEW_LIBRARY
  GLEW_INCLUDE_DIR
)

IF(NOT GLEW_FOUND)
  MESSAGE("GLEW library not found.")
ENDIF(NOT GLEW_FOUND)

