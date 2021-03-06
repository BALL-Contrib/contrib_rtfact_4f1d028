#
# Find the RTSG includes and library
##
# sets RTSG_INCLUDE_DIR
#      RTSG_LIBRARY
#      RTSG_DEBUG_LIBRARY
#      RTSG_FOUND
#      RTSG_LIBRARIES
#      RTSG_LIBRARY_DIR

FIND_PATH(RTSG_INCLUDE_DIR rtsg/rtsg.hpp
  ${RTSG_ROOT}/include
  /usr/local/include
  /usr/include
)


FIND_LIBRARY(RTSG_LIBRARY
           NAMES rtsg
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGVRML_LIBRARY
           NAMES rtsgvrml
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGGLUTIL_LIBRARY
           NAMES rtsgglutil
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGRTREMOTE_LIBRARY
           NAMES rtsgrtremote
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)

FIND_LIBRARY(RTSG_LIBRARY_DEBUG
           NAMES rtsg
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGVRML_LIBRARY_DEBUG
           NAMES rtsgvrml
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGGLUTIL_LIBRARY_DEBUG
           NAMES rtsgglutil
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)
FIND_LIBRARY(RTSGRTREMOTE_LIBRARY_DEBUG
           NAMES rtsgrtremote
           PATHS ${RTSG_INCLUDE_DIR}/../lib
           ${RTSG_INCLUDE_DIR}/../lib/${ARCH}
           /usr/local/lib
           /usr/lib)


SET(RTSG_FOUND "NO")
IF(RTSG_INCLUDE_DIR AND RTSG_LIBRARY AND RTSGVRML_LIBRARY AND RTSGGLUTIL_LIBRARY AND RTSGRTREMOTE_LIBRARY)
  GET_FILENAME_COMPONENT( RTSG_LIBRARY_DIR ${RTSG_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( RTSG_LIBRARY ${RTSG_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( RTSGVRML_LIBRARY ${RTSGVRML_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( RTSGGLUTIL_LIBRARY ${RTSGGLUTIL_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( RTSGRTREMOTE_LIBRARY ${RTSGRTREMOTE_LIBRARY} NAME )
  
  IF(WIN32)
    SET(RTSG_LIBRARIES optimized ${RTSG_LIBRARY}
                       optimized ${RTSGVRML_LIBRARY}
                       optimized ${RTSGGLUTIL_LIBRARY}
                       optimized ${RTSGRTREMOTE_LIBRARY})
    IF(RTSG_LIBRARY_DEBUG AND RTSGVRML_LIBRARY_DEBUG AND RTSGGLUTIL_LIBRARY_DEBUG AND RTSGRTREMOTE_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( RTSG_LIBRARY_DEBUG ${RTSG_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( RTSGVRML_LIBRARY_DEBUG ${RTSGVRML_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( RTSGGLUTIL_LIBRARY_DEBUG ${RTSGGLUTIL_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( RTSGRTREMOTE_LIBRARY_DEBUG ${RTSGRTREMOTE_LIBRARY_DEBUG} NAME )
      SET(RTSG_LIBRARIES ${RTSG_LIBRARIES} debug ${RTSG_LIBRARY_DEBUG}
                                           debug ${RTSGVRML_LIBRARY_DEBUG}
                                           debug ${RTSGGLUTIL_LIBRARY_DEBUG}
                                           debug ${RTSGRTREMOTE_LIBRARY_DEBUG} )
    ENDIF(RTSG_LIBRARY_DEBUG AND RTSGVRML_LIBRARY_DEBUG AND RTSGGLUTIL_LIBRARY_DEBUG AND RTSGRTREMOTE_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(RTSG_LIBRARIES general ${RTSG_LIBRARY}
                       general ${RTSGVRML_LIBRARY}
                       general ${RTSGGLUTIL_LIBRARY}
                       general ${RTSGRTREMOTE_LIBRARY})
  ENDIF(WIN32)
  SET(RTSG_FOUND "YES")
ENDIF(RTSG_INCLUDE_DIR AND RTSG_LIBRARY AND RTSGVRML_LIBRARY AND RTSGGLUTIL_LIBRARY AND RTSGRTREMOTE_LIBRARY)

SET(RTSG_LIBRARIES "${RTSG_LIBRARIES}" CACHE STRING "List of RTSG libraries")
SET(RTSG_LIBRARY_DIRS "${RTSG_LIBRARY_DIR}" CACHE STRING "Paths to RTSG libraries")
