#
# Find the PNG includes and library
##
# sets PNG_INCLUDE_DIR
#      PNG_LIBRARY
#      PNG_DEBUG_LIBRARY
#      PNG_FOUND
#      PNG_LIBRARIES
#      PNG_LIBRARY_DIR

FIND_PATH(PNG_INCLUDE_DIR png.h
  ${PNG_ROOT}/include
  /usr/local/include
  /usr/include
)


IF(WIN32)
  FIND_LIBRARY(PNG_LIBRARY
               NAMES libpng
               PATHS ${PNG_INCLUDE_DIR}/../lib
               ${PNG_INCLUDE_DIR}/../lib/${ARCH})
  FIND_LIBRARY(PNG_LIBRARY_DEBUG
               NAMES libpng_debug libpngd libpng
               PATHS ${PNG_INCLUDE_DIR}/../lib
               ${PNG_INCLUDE_DIR}/../lib/${ARCH})
ELSE(WIN32)
  FIND_LIBRARY(PNG_LIBRARY libpng
               ${PNG_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(PNG_LIBRARY_DEBUG libpng_debug libpng
               ${PNG_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)


SET(PNG_FOUND "NO")
IF(PNG_INCLUDE_DIR AND PNG_LIBRARY)
  GET_FILENAME_COMPONENT( PNG_LIBRARY_DIR ${PNG_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( PNG_LIBRARY ${PNG_LIBRARY} NAME )
  IF(WIN32)
    SET(PNG_LIBRARIES optimized ${PNG_LIBRARY} ${CMAKE_DL_LIBS})
    IF(PNG_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( PNG_LIBRARY_DEBUG ${PNG_LIBRARY_DEBUG} NAME )
      SET(PNG_LIBRARIES ${PNG_LIBRARIES} debug ${PNG_LIBRARY_DEBUG})
    ENDIF(PNG_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(PNG_LIBRARIES general ${PNG_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(WIN32)
  SET(PNG_FOUND "YES")
ENDIF(PNG_INCLUDE_DIR AND PNG_LIBRARY)

SET(PNG_LIBRARIES "${PNG_LIBRARIES}" CACHE STRING "List of PNG libraries")

