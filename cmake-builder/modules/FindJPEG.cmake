#
# Find the JPEG includes and library
##
# sets JPEG_INCLUDE_DIR
#      JPEG_LIBRARY
#      JPEG_DEBUG_LIBRARY
#      JPEG_FOUND
#      JPEG_LIBRARIES
#      JPEG_LIBRARY_DIR

FIND_PATH(JPEG_INCLUDE_DIR jpeglib.h
  ${JPEG_ROOT}/include
  /usr/local/include
  /usr/include
)


IF(WIN32)
  FIND_LIBRARY(JPEG_LIBRARY
               NAMES jpeg
               PATHS ${JPEG_INCLUDE_DIR}/../lib
               ${JPEG_INCLUDE_DIR}/../lib/${ARCH})
  FIND_LIBRARY(JPEG_LIBRARY_DEBUG
               NAMES jpegd jpeg_debug jpeg
               PATHS ${JPEG_INCLUDE_DIR}/../lib
               ${JPEG_INCLUDE_DIR}/../lib/${ARCH})
ELSE(WIN32)
  FIND_LIBRARY(JPEG_LIBRARY 
               NAMES jpeg
               PATHS ${JPEG_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(JPEG_LIBRARY_DEBUG 
               NAMES jpeg_debug jpegd jpeg
               PATHS ${JPEG_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)


SET(JPEG_FOUND "NO")
IF(JPEG_INCLUDE_DIR AND JPEG_LIBRARY)
  GET_FILENAME_COMPONENT( JPEG_LIBRARY_DIR ${JPEG_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( JPEG_LIBRARY ${JPEG_LIBRARY} NAME )
  IF(WIN32)
    SET(JPEG_LIBRARIES optimized ${JPEG_LIBRARY} ${CMAKE_DL_LIBS})
    IF(JPEG_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( JPEG_LIBRARY_DEBUG ${JPEG_LIBRARY_DEBUG} NAME )
      SET(JPEG_LIBRARIES ${JPEG_LIBRARIES} debug ${JPEG_LIBRARY_DEBUG})
    ENDIF(JPEG_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(JPEG_LIBRARIES general ${JPEG_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(WIN32)
  SET(JPEG_FOUND "YES")
ENDIF(JPEG_INCLUDE_DIR AND JPEG_LIBRARY)

SET(JPEG_LIBRARIES "${JPEG_LIBRARIES}" CACHE STRING "List of JPEG libraries")

