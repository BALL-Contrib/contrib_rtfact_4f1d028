#
# Find the DFC includes and library
##
# sets DFC_INCLUDE_DIR
#      DFC_LIBRARY
#      DFC_DEBUG_LIBRARY
#      DFC_FOUND
#      DFC_LIBRARIES
#      DFC_LIBRARY_DIR

FIND_PATH(DFC_INCLUDE_DIR DFC/Common/Config.hpp
  ${DFC_ROOT}/include
  /usr/local/include
  /usr/include
)


FIND_LIBRARY(DFC_LIBRARY
             NAMES DFC
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)
FIND_LIBRARY(DFC_CDD_LIBRARY
             NAMES DFCCDD
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)
FIND_LIBRARY(DFC_ImageIO_LIBRARY
             NAMES DFCImageIO
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)

#debug
FIND_LIBRARY(DFC_LIBRARY_DEBUG
             NAMES DFC
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)
FIND_LIBRARY(DFC_CDD_LIBRARY_DEBUG
             NAMES DFCCDD
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)
FIND_LIBRARY(DFC_ImageIO_LIBRARY_DEBUG
             NAMES DFCImageIO
             PATHS ${DFC_INCLUDE_DIR}/../lib
             ${DFC_INCLUDE_DIR}/../lib/${ARCH}
             /usr/local/lib
             /usr/lib)

SET(DFC_FOUND "NO")
IF(DFC_INCLUDE_DIR AND DFC_LIBRARY AND DFC_CDD_LIBRARY AND DFC_ImageIO_LIBRARY)
  GET_FILENAME_COMPONENT( DFC_LIBRARY_DIR ${DFC_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( DFC_LIBRARY ${DFC_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( DFC_CDD_LIBRARY ${DFC_CDD_LIBRARY} NAME )
  GET_FILENAME_COMPONENT( DFC_ImageIO_LIBRARY ${DFC_ImageIO_LIBRARY} NAME )
  IF(WIN32)
    SET(DFC_LIBRARIES optimized ${DFC_LIBRARY}
                      optimized ${DFC_CDD_LIBRARY}
                      optimized ${DFC_ImageIO_LIBRARY})
    IF(DFC_LIBRARY_DEBUG AND DFC_CDD_LIBRARY_DEBUG AND DFC_ImageIO_LIBRARY_DEBUG)

      GET_FILENAME_COMPONENT( DFC_LIBRARY_DEBUG ${DFC_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( DFC_CDD_LIBRARY_DEBUG ${DFC_CDD_LIBRARY_DEBUG} NAME )
      GET_FILENAME_COMPONENT( DFC_ImageIO_LIBRARY_DEBUG ${DFC_ImageIO_LIBRARY_DEBUG} NAME )

      SET(DFC_LIBRARIES ${DFC_LIBRARIES}
                        debug ${DFC_LIBRARY_DEBUG}
                        debug ${DFC_CDD_LIBRARY_DEBUG}
                        debug ${DFC_ImageIO_LIBRARY_DEBUG})
    ENDIF(DFC_LIBRARY_DEBUG AND DFC_CDD_LIBRARY_DEBUG AND DFC_ImageIO_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(DFC_LIBRARIES general ${DFC_LIBRARY}
                      general ${DFC_CDD_LIBRARY}
                      general ${DFC_ImageIO_LIBRARY})
  ENDIF(WIN32)
  SET(DFC_FOUND "YES")
ENDIF(DFC_INCLUDE_DIR AND DFC_LIBRARY AND DFC_CDD_LIBRARY AND DFC_ImageIO_LIBRARY)

SET(DFC_LIBRARIES "${DFC_LIBRARIES}" CACHE STRING "List of DFC libraries")
SET(DFC_LIBRARY_DIRS "${DFC_LIBRARY_DIR}" "${DFC_LIBRARY_DIR_DEBUG}" CACHE STRING "Path to DFC libraries")