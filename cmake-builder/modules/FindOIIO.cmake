#
# Find the OIIO includes and library
##
# sets OIIO_INCLUDE_DIR
#      OIIO_LIBRARY
#      OIIO_DEBUG_LIBRARY
#      OIIO_FOUND
#      OIIO_LIBRARIES
#      OIIO_LIBRARY_DIR

FIND_PATH(OIIO_INCLUDE_DIR OpenImageIO/imageio.h
  ${OIIO_ROOT}/src/include
  ${OIIO_ROOT}/include
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(OIIO_LIBRARY
           NAMES OpenImageIO
           PATHS ${OIIO_INCLUDE_DIR}/../lib
           ${OIIO_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)

FIND_LIBRARY(OIIO_LIBRARY_DEBUG
           NAMES OpenImageIO
           PATHS ${OIIO_INCLUDE_DIR}/../lib
           ${OIIO_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)

SET(OIIO_FOUND "NO")
IF(OIIO_INCLUDE_DIR AND OIIO_LIBRARY)
  GET_FILENAME_COMPONENT( OIIO_LIBRARY_DIR ${OIIO_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( OIIO_LIBRARY ${OIIO_LIBRARY} NAME )
  
  IF(WIN32)
    SET(OIIO_LIBRARIES optimized ${OIIO_LIBRARY})
    IF(OIIO_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( OIIO_LIBRARY_DEBUG ${OIIO_LIBRARY_DEBUG} NAME )
      SET(OIIO_LIBRARIES ${OIIO_LIBRARIES} debug ${OIIO_LIBRARY_DEBUG} )
    ENDIF(OIIO_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(OIIO_LIBRARIES general ${OIIO_LIBRARY})
  ENDIF(WIN32)
  SET(OIIO_FOUND "YES")
ENDIF(OIIO_INCLUDE_DIR AND OIIO_LIBRARY)


SET(OIIO_LIBRARIES "${OIIO_LIBRARIES}" CACHE STRING "List of OIIO libraries")
SET(OIIO_LIBRARY_DIRS "${OIIO_LIBRARY_DIR}" CACHE STRING "Paths to OIIO libraries")

