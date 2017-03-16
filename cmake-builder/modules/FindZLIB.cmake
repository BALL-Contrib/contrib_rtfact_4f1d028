#
# Find the ZLIB includes and library
##
# sets ZLIB_INCLUDE_DIR
#      ZLIB_LIBRARY
#      ZLIB_DEBUG_LIBRARY
#      ZLIB_FOUND
#      ZLIB_LIBRARIES
#      ZLIB_LIBRARY_DIR

FIND_PATH(ZLIB_INCLUDE_DIR zlib.h
  ${ZLIB_ROOT}/include
  /usr/local/include
  /usr/include
)


IF(WIN32)
  FIND_LIBRARY(ZLIB_LIBRARY
               NAMES zlib
               PATHS ${ZLIB_INCLUDE_DIR}/../lib
               ${ZLIB_INCLUDE_DIR}/../lib/${ARCH})
  FIND_LIBRARY(ZLIB_LIBRARY_DEBUG
               NAMES zlib_debug zlibd zlib
               PATHS ${ZLIB_INCLUDE_DIR}/../lib
               ${ZLIB_INCLUDE_DIR}/../lib/${ARCH})
ELSE(WIN32)
  FIND_LIBRARY(ZLIB_LIBRARY 
               NAMES zlib
               PATHS ${ZLIB_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(ZLIB_LIBRARY_DEBUG 
               NAMES zlib_debug zlibd zlib
               PATHS ${ZLIB_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)


SET(ZLIB_FOUND "NO")
IF(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
  GET_FILENAME_COMPONENT( ZLIB_LIBRARY_DIR ${ZLIB_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( ZLIB_LIBRARY ${ZLIB_LIBRARY} NAME )
  IF(WIN32)
    SET(ZLIB_LIBRARIES optimized ${ZLIB_LIBRARY} ${CMAKE_DL_LIBS})
    IF(ZLIB_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( ZLIB_LIBRARY_DEBUG ${ZLIB_LIBRARY_DEBUG} NAME )
      SET(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} debug ${ZLIB_LIBRARY_DEBUG})
    ENDIF(ZLIB_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(ZLIB_LIBRARIES general ${ZLIB_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(WIN32)
  SET(ZLIB_FOUND "YES")
ENDIF(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)

SET(ZLIB_LIBRARIES "${ZLIB_LIBRARIES}" CACHE STRING "List of ZLIB libraries")

