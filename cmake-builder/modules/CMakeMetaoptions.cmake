#
# Defines:
#   ARCH
#   DEBUG
#   OPTIMIZE


# Variables settings
################################################################################

IF( CMAKE_SIZEOF_VOID_P EQUAL 8)
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8)

IF(CMAKE_CL_64)
  SET(ARCH x64)
ELSE(CMAKE_CL_64)
  SET(ARCH x86)
ENDIF(CMAKE_CL_64)

IF(WIN32)
  SET(OPT_OPTIMIZE "/O2 /Ob2")
  SET(OPT_DEBUG "/Od /RTC1 /Ob0")
  SET(TRACEABLE "/Zi")
  SET(NO_UNKNOWN_PRAGMAS "")
  SET(RT_DYNAMIC_DEBUG "/MDd")
  SET(RT_DYNAMIC "/MD")
  SET(RT_STATIC_DEBUG "/MTd")
  SET(RT_STATIC "/MT")
  SET(LINK_DEBUG "/DEBUG /INCREMENTAL:YES")
  SET(LINK_RELEASE "/INCREMENTAL:NO")
ENDIF(WIN32)

IF("${COMPILER}" MATCHES "^([iI][nN][tT][eE][lL])$" OR "${COMPILER}" MATCHES "^([iI][cC][cC])$")
    SET(CMAKE_C_COMPILER "icc")
ELSEIF("${COMPILER}" MATCHES "^([gG][cC][cC])$")
    SET(CMAKE_C_COMPILER "gcc")
ELSEIF("${COMPILER}" MATCHES "^([cC][lL][aA][nN][gG])$")
    SET(CMAKE_C_COMPILER "clang")
ENDIF("${COMPILER}" MATCHES "^([iI][nN][tT][eE][lL])$" OR "${COMPILER}" MATCHES "^([iI][cC][cC])$")

string(REGEX MATCH ".*gcc$" GCC ${CMAKE_C_COMPILER} )

IF(UNIX AND GCC)
  SET(OPT_OPTIMIZE "-O2")
  SET(OPT_DEBUG "-O0")
  SET(TRACEABLE "-g")
  SET(SSE "-msse -msse2")
  SET(NO_UNKNOWN_PRAGMAS "-Wno-unknown-pragmas")
ENDIF(UNIX AND GCC)

string(REGEX MATCH ".*icc$" ICC ${CMAKE_C_COMPILER} )

string(REGEX MATCH ".*clang$" CLANG ${CMAKE_C_COMPILER} )

IF(UNIX AND ICC)
  SET(OPT_OPTIMIZE "-O3")
  SET(OPT_DEBUG "-O0")
  SET(TRACEABLE "-g")
  SET(NO_UNKNOWN_PRAGMAS "-Wno-unknown-pragmas")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ENDIF(UNIX AND ICC)

IF(GCC OR ICC)
  IF(TARGET_ARCH STREQUAL "x86")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  ELSEIF(TARGET_ARCH STREQUAL "x86_64")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
  ENDIF(TARGET_ARCH STREQUAL "x86")
ENDIF(GCC OR ICC)

# Change native compiler
################################################################################

IF(UNIX AND ICC)
  INCLUDE (CMakeForceCompiler)
  CMAKE_FORCE_C_COMPILER   (icc IntelC)
  CMAKE_FORCE_CXX_COMPILER (icpc IntelCPP)
ENDIF(UNIX AND ICC)

IF(UNIX AND CLANG)
  INCLUDE (CMakeForceCompiler)
  CMAKE_FORCE_C_COMPILER   (clang CLang)
  CMAKE_FORCE_CXX_COMPILER (clang++ CLang)
ENDIF(UNIX AND CLANG)


# definitions setting wrapper functions
################################################################################

FUNCTION(canonizePath path canonizedPath)

  get_filename_component(absolutePath ${path} ABSOLUTE)
  file(TO_CMAKE_PATH ${absolutePath} ${canonizedPath})
  set(${canonizedPath} ${${canonizedPath}} PARENT_SCOPE)

ENDFUNCTION(canonizePath)

FUNCTION(set_compile_definitions def)

  set_property(DIRECTORY . PROPERTY COMPILE_DEFINITIONS ${def})

ENDFUNCTION(set_compile_definitions)

FUNCTION(append_compile_definitions def)

  set_property(DIRECTORY . APPEND PROPERTY COMPILE_DEFINITIONS ${def})

ENDFUNCTION(append_compile_definitions)


FUNCTION(set_config_compile_definitions conf def)

  string(TOUPPER ${conf} upconf)
  set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS_${upconf} ${def})

ENDFUNCTION(set_config_compile_definitions)



FUNCTION(set_target_compile_definitions target def)

  set_property(TARGET ${target} PROPERTY COMPILE_DEFINITIONS ${def})

ENDFUNCTION(set_target_compile_definitions)



FUNCTION(set_target_config_compile_definitions target conf def)

  string(TOUPPER ${conf} upconf)
  set_property(TARGET ${target} PROPERTY COMPILE_DEFINITIONS_${upconf} ${def})

ENDFUNCTION(set_target_config_compile_definitions)



FUNCTION(set_file_config_compile_definitions file conf def)

  canonizePath(${file} canonizedFileName)
  string(TOUPPER ${conf} upconf)
  set_property(SOURCE ${canonizedFileName} PROPERTY COMPILE_DEFINITIONS_${upconf} ${def})

ENDFUNCTION(set_file_config_compile_definitions)


# compile flags setting wrapper functions
################################################################################

FUNCTION(add_compile_flags flag)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)

ENDFUNCTION(add_compile_flags)



FUNCTION(add_target_compile_flags target flag)

  get_property(old TARGET ${target} PROPERTY COMPILE_FLAGS)
  set_property(TARGET ${target} PROPERTY COMPILE_FLAGS "${old} ${flag}")

ENDFUNCTION(add_target_compile_flags)



FUNCTION(add_config_compile_flags conf flag)

  string(TOUPPER ${conf} upconf)
  SET(CMAKE_CXX_FLAGS_${upconf} "${CMAKE_CXX_FLAGS_${upconf}} ${flag}" PARENT_SCOPE)
  SET(CMAKE_C_FLAGS_${upconf} "${CMAKE_C_FLAGS_${upconf}} ${flag}" PARENT_SCOPE)

ENDFUNCTION(add_config_compile_flags)


