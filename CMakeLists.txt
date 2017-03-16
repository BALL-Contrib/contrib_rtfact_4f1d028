CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Builder initialization
###############################################################################
IF(WIN32)
    SET(BUILDER_SCRIPT_FILE "initbuilder.bat")
ELSE(WIN32)
    SET(BUILDER_SCRIPT_FILE "./initbuilder.sh")
ENDIF(WIN32)

EXECUTE_PROCESS(COMMAND ${BUILDER_SCRIPT_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE INIT_BUILDER_RESULT
        OUTPUT_VARIABLE INIT_BUILDER_MESSAGES
        ERROR_VARIABLE INIT_BUILDER_MESSAGES)

IF(NOT INIT_BUILDER_RESULT EQUAL 0)
    MESSAGE(FATAL_ERROR "Cannot clone and intialize cmake-builder.\nError code: ${INIT_BUILDER_RESULT}\nError message: ${INIT_BUILDER_MESSAGES}")
ENDIF(NOT INIT_BUILDER_RESULT EQUAL 0)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-builder/modules/")

#<?EKAMC@ Configurations
###############################################################################

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

SET_PROPERTY(GLOBAL PROPERTY DEBUG_CONFIGURATIONS DEBUG)

#@?>

#<?EKAMC@ Project
###############################################################################
PROJECT( rtfact )

INCLUDE( CMakeMetaoptions )
ADD_CONFIG_COMPILE_FLAGS(DEBUG "${OPT_DEBUG} ${TRACEABLE} ${RT_DYNAMIC_DEBUG} ${SSE} ${NO_UNKNOWN_PRAGMAS} ${NO_DEPRECATED}")
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${LINK_DEBUG}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${LINK_DEBUG}")

ADD_CONFIG_COMPILE_FLAGS(RELEASE "${OPT_OPTIMIZE} ${RT_DYNAMIC} ${SSE} ${NO_UNKNOWN_PRAGMAS} ${NO_DEPRECATED}")
SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${LINK_RELEASE}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINK_RELEASE}")

IF(UNIX AND ICC)
  ADD_CONFIG_COMPILE_FLAGS(DEBUG "-wd858 -wd1931")
  ADD_CONFIG_COMPILE_FLAGS(RELEASE "-wd858 -wd1931")
ENDIF(UNIX AND ICC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

#global definitions

# RTfactConfig.hpp has to included manually by application
# SET_COMPILE_DEFINITIONS("RTFACT_USER_CONFIG_FILE=RTfactConfig.hpp;_CONSOLE;_CRT_SECURE_NO_WARNINGS")
#SET_COMPILE_DEFINITIONS("_CONSOLE;_CRT_SECURE_NO_WARNINGS")
SET_COMPILE_DEFINITIONS("RTFACT_USER_CONFIG_FILE=RTfact/Config/RTfactConfig.default.hpp;_CONSOLE;_CRT_SECURE_NO_WARNINGS")

IF(WIN32)
  STRING(REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
ENDIF(WIN32)

#@?>

IF(WIN32)
  ADD_COMPILE_FLAGS("/wd4503 /wd4251 /wd4275")
  #disable VS warning 4503 (identifier truncation)
  #disable VS warning 4251 (Base class needs to have dll-interface to be used by clients of derived class)
  #disable VS warning 4275 (An exported class was derived from a class that was not exported)
  #do it like this until a warning-handling metaoption is available
ENDIF(WIN32)

# User Definitions
###############################################################################

INCLUDE(UserCMakeLists.txt OPTIONAL)

# Libraries
###############################################################################

INCLUDE( CMakePCHSupport )
INCLUDE( CMakeSourceTree )

#Lists for optional libraries. Optional libraries should be appended conditionally to this
#list, if they are found

SET(OPTIONAL_LIB_INCLUDE_DIRS "")
SET(OPTIONAL_LIB_LIBRARY_DIRS "")
SET(OPTIONAL_LIBRARIES "")

FIND_PACKAGE( Boost 1.43 REQUIRED )
FIND_PACKAGE( TBB )
FIND_PACKAGE( SDL )
FIND_PACKAGE( OpenGL )
FIND_PACKAGE( GLEW )
FIND_PACKAGE( Cg )
IF(NOT CG_FOUND)
    SET(RTREMOTE_USE_CGFX_POSTPROCESSOR, FALSE)
ELSE(NOT CG_FOUND)
    SET(OPTIONAL_LIB_INCLUDE ${OPTIONAL_LIB_INCLUDE} ${CG_INCLUDE_DIRS})
    SET(OPTIONAL_LIB_LIBRARY_DIRS ${OPTIONAL_LIB_LIBRARY_DIRS} ${CG_LIBRARY_DIRS})
    SET(OPTIONAL_LIB_LIBRARIES ${OPTIONAL_LIB_LIBRARIES} ${CG_LIBRARIES})
ENDIF(NOT CG_FOUND)
FIND_PACKAGE( JITRT )
IF(NOT JITRT_FOUND)
    SET(RTFACT_REMOTE_HAVE_ANYSL, FALSE)
ELSE(NOT JITRT_FOUND)
    SET(OPTIONAL_LIB_INCLUDE ${OPTIONAL_LIB_INCLUDE} ${JITRT_INCLUDE_DIR})
    SET(OPTIONAL_LIB_LIBRARY_DIRS ${OPTIONAL_LIB_LIBRARY_DIRS} ${JITRT_LIBRARY_DIR})
    SET(OPTIONAL_LIB_LIBRARIES ${OPTIONAL_LIB_LIBRARIES} ${JITRT_LIBRARIES})
ENDIF(NOT JITRT_FOUND)

#Remove/Add the CgFX postprocessor-related files from compilation of RTremote
IF(RTREMOTE_USE_CGFX_POSTPROCESSOR)
    MESSAGE(STATUS "CgFX postprocessor: ON")
    APPEND_COMPILE_DEFINITIONS("USE_CGFX_POSTPROCESSOR")
ELSE(RTREMOTE_USE_CGFX_POSTPROCESSOR)
    MESSAGE(STATUS "CgFX postprocessor: OFF")
    IF(rtfactremoteexcludepattern)
        set(rtfactremoteexcludepattern "${rtfactremoteexcludepattern}|Postprocessor.cpp|PostprocessorParameter.cpp|Effect.cpp")
    ELSE(rtfactremoteexcludepattern)
        set(rtfactremoteexcludepattern "Postprocessor.cpp|PostprocessorParameter.cpp|Effect.cpp")
    ENDIF(rtfactremoteexcludepattern)
ENDIF(RTREMOTE_USE_CGFX_POSTPROCESSOR)

IF(RTFACT_REMOTE_HAVE_ANYSL)
    MESSAGE(STATUS "AnySL extension: ON")
    APPEND_COMPILE_DEFINITIONS("RTFACT_REMOTE_HAVE_ANYSL")
ELSE(RTFACT_REMOTE_HAVE_ANYSL)
    MESSAGE(STATUS "AnySL extension: OFF")
    IF(rtfactremoteexcludepattern)
        set(rtfactremoteexcludepattern "${rtfactremoteexcludepattern}|RTAnySL|CXXShaderManager.cpp|RSLShaderManager.cpp|RTAnySLShader.cpp|RTfactShaderEntryTableMaker.cpp")
    ELSE(rtfactremoteexcludepattern)
        set(rtfactremoteexcludepattern "RTAnySL|CXXShaderManager.cpp|RSLShaderManager.cpp|RTAnySLShader.cpp|RTfactShaderEntryTableMaker.cpp")
    ENDIF(rtfactremoteexcludepattern)
ENDIF(RTFACT_REMOTE_HAVE_ANYSL)

#This exclusion of Precompiled.cpp is a hack, precompiled headers get messed up without it
IF(rtfactremoteexcludepattern)
    set(rtfactremoteexcludepattern "${rtfactremoteexcludepattern}|Precompiled.cpp")
ELSE(rtfactremoteexcludepattern)
    set(rtfactremoteexcludepattern "Precompiled.cpp")
ENDIF(rtfactremoteexcludepattern)

#<?EKAMC@ Sources
##############################################################################

build_source_tree( "./include"
                   "."
                   ""
                   rtfactheaders
                   "*.h*",
                   "")
build_source_tree( "./remote/src"
                   "./remote/src"
                   "remote\\include"
                   rtfactremoteheaders
                   "*.h*"
                   "")
build_source_tree( "./remote/src"
                   "./remote/src"
                   "remote\\src"
                   rtfactremotesources
                   "*.c*"
                   ${rtfactremoteexcludepattern})
build_source_tree( "./demos/RTfactRemoteDemo/src"
                   "./demos/RTfactRemoteDemo/src"
                   "include"
                   rtremotedemoheaders
                   "*.h*"
                   "")
build_source_tree( "./demos/RTfactRemoteDemo/src"
                   "./demos/RTfactRemoteDemo/src"
                   "src"
                   rtremotedemosources
                   "*.c*"
                   "")
build_source_tree( "./demos/RTfactRawDemo/src"
                   "./demos/RTfactRawDemo/src"
                   "include"
                   rawdemoheaders
                   "*.h*"
                   "")
build_source_tree( "./demos/RTfactRawDemo/src"
                   "./demos/RTfactRawDemo/src"
                   "src"
                   rawdemosources
                   "*.c*"
                   "Precompiled.cpp")
#the above exclusion of Precompiled.cpp is a hack, precompiled headers get messed up without it

build_source_tree( "./rtpie/src"
                   "./rtpie/src"
                   "src"
                   rtfactrtpieheaders
                   "*.h*"
                   "")
build_source_tree( "./rtpie/src"
                   "./rtpie/src"
                   "src"
                   rtfactrtpiesources
                   "*.c*"
                   "")

build_source_tree( "./demos/RTpieDemo/src"
                   "./demos/RTpieDemo/src"
                   "src"
                   rtpiedemoheaders
                   "*.h*"
                   "")
build_source_tree( "./demos/RTpieDemo/src"
                   "./demos/RTpieDemo/src"
                   "src"
                   rtpiedemosources
                   "*.c*"
                   "")

build_source_tree( "./shaders/src"
                   "./shaders/"
                   "shaders"
                   rtfactanyslshaders
                   "*.cpp"
                   "")

build_source_tree( "./shaders/src/rsl"
                   "./shaders/"
                   "shaders"
                   rtfactanyslrsl
                   "*.sl"
                   "dented;paintedplastic;shinymetal")

build_source_tree( "./gadgets"
                   "./gadgets/"
                   "gadgets"
                   rtfactanyslgadgets
                   "*.cpp"
                   "")

build_source_tree( "./gadgets"
                   "./gadgets"
                   "gadgets"
                   rtfactanyslgadgetsheaders
                   "*.h"
                   "")

set(RTfactRemote_SOURCE_FILES ${rtfactheaders}
                              ${rtfactremoteheaders}
                              ${rtfactremotesources}
                              ./remote/src/Precompiled.cpp)

set(RTfactRTpie_SOURCE_FILES ${rtfactheaders}
                             ${rtfactrtpieheaders}
                             ${rtfactrtpiesources})

set(RTfactRemoteDemo_SOURCE_FILES ${rtremotedemoheaders}
                                  ${rtremotedemosources})

set(RTfactRTpieDemo_SOURCE_FILES ${rtpiedemoheaders}
                               ${rtpiedemosources})

set(RTfactRawDemo_SOURCE_FILES ${rawdemoheaders}
                               ${rawdemosources}
                               ./demos/RTfactRawDemo/src/Precompiled.cpp)
#@?>

#<?EKAMC@ File_configurations
###############################################################################

#set_file_config_compile_definitions(./demos/RTfactRawDemo/src/AccelStructDebug.cpp "DEBUG" "EXCLUDE_FROM_BUILD")
#set_file_config_compile_definitions(./demos/RTfactRawDemo/src/AccelStructDebug.cpp "RELEASE" "EXCLUDE_FROM_BUILD")

#@?>

#<?EKAMC@ Paths
###############################################################################

INCLUDE_DIRECTORIES( ./remote )
INCLUDE_DIRECTORIES( ./remote/src )
INCLUDE_DIRECTORIES( ./rtpie/src )
INCLUDE_DIRECTORIES( ./include )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${TBB_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${SDL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenGL_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OPTIONAL_LIB_INCLUDE} )


LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )
LINK_DIRECTORIES( ${TBB_LIBRARY_DIRS} )
LINK_DIRECTORIES( ${SDL_LIBRARY_DIRS} )
LINK_DIRECTORIES( ${OpenGL_LIBRARY_DIRS} )
LINK_DIRECTORIES( ${GLEW_LIBRARY_DIR} )
LINK_DIRECTORIES( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
LINK_DIRECTORIES( ${OPTIONAL_LIB_LIBRARY_DIRS} )


#@?>

#<?EKAMC@ Targets
###############################################################################

#ADD_LIBRARY(RTfactRemote SHARED ${RTfactRemote_SOURCE_FILES})
#SET_PRECOMPILED_HEADER(RTfactRemote ./remote/src/Precompiled.hpp ./remote/src/Precompiled.cpp)
#TARGET_LINK_LIBRARIES(RTfactRemote  ${TBB_LIBRARIES}
#                                    ${OPENGL_LIBRARIES}
#                                    ${GLEW_LIBRARIES}
#                                    ${OPTIONAL_LIB_LIBRARIES})
#SET_TARGET_COMPILE_DEFINITIONS(RTfactRemote LIB)
#
#ADD_EXECUTABLE(RTfactRemoteDemo ${RTfactRemoteDemo_SOURCE_FILES})
#TARGET_LINK_LIBRARIES(RTfactRemoteDemo ${TBB_LIBRARIES}
#                                       ${SDL_LIBRARIES}
#                                       ${OPENGL_LIBRARIES}
#                                       ${GLEW_LIBRARIES}
#                                       ${GDIPLUS_LIBRARIES}
#                                       ${OPTIONAL_LIB_LIBRARIES}
#                                       RTfactRemote)
#SET_TARGET_COMPILE_DEFINITIONS(RTfactRemoteDemo DEMO)



ADD_LIBRARY(RTfactRTpie SHARED ${RTfactRTpie_SOURCE_FILES})
TARGET_LINK_LIBRARIES(RTfactRTpie ${TBB_LIBRARIES} )
SET_TARGET_COMPILE_DEFINITIONS(RTfactRTpie LIB)

ADD_EXECUTABLE(RTpieDemo ${RTfactRTpieDemo_SOURCE_FILES})
TARGET_LINK_LIBRARIES(RTpieDemo ${TBB_LIBRARIES}
                                       ${SDL_LIBRARIES}
                                       ${OPENGL_LIBRARIES}
                                       ${GLEW_LIBRARIES}
                                       RTfactRTpie)
SET_TARGET_COMPILE_DEFINITIONS(RTpieDemo DEMO)

#RTfactRawDemo uses windows-only gdiplus libraries
IF(WIN32)
  ADD_EXECUTABLE(RTfactRawDemo ${RTfactRawDemo_SOURCE_FILES})
  SET_PRECOMPILED_HEADER(RTfactRawDemo ./demos/RTfactRawDemo/src/Precompiled.hpp ./demos/RTfactRawDemo/src/Precompiled.cpp)
  TARGET_LINK_LIBRARIES(RTfactRawDemo   ${TBB_LIBRARIES}
                                        ${SDL_LIBRARIES}
                                        ${OPENGL_LIBRARIES}
                                        ${GLEW_LIBRARIES}
                                        gdiplus.lib)
ENDIF(WIN32)

IF(RTFACT_REMOTE_HAVE_ANYSL)
  #NOTE: some paths have to be in "native format" (these that are executed in native environment, COMMAND
  #section of add_custom_commands, -I are fine with "cmake format") and some in "cmake format" (with /s,
  #these that are used in cmake commands).

  # Check tools

  IF( NOT DEFINED SDRC OR NOT (EXISTS "${SDRC}" OR EXISTS "${SDRC}.exe"))
    MESSAGE( FATAL_ERROR "SDRC is not defined or does not exist" )
  ENDIF( NOT DEFINED SDRC OR NOT (EXISTS "${SDRC}" OR EXISTS "${SDRC}.exe"))

  IF( NOT DEFINED LLVM_GCC OR NOT (EXISTS "${LLVM_GCC}" OR EXISTS "${LLVM_GCC}.exe"))
    MESSAGE( FATAL_ERROR "LLVM_GCC is not defined or does not exist" )
  ENDIF( NOT DEFINED LLVM_GCC OR NOT (EXISTS "${LLVM_GCC}" OR EXISTS "${LLVM_GCC}.exe"))

  IF( NOT DEFINED SDR2BC OR NOT (EXISTS "${SDR2BC}" OR EXISTS "${SDR2BC}.exe"))
    MESSAGE( FATAL_ERROR "SDR2BC is not defined or does not exist" )
  ENDIF( NOT DEFINED SDR2BC OR NOT (EXISTS "${SDR2BC}" OR EXISTS "${SDR2BC}.exe"))

  # Shaders

  SET(LLVM_CFLAGS -O3 -Wall -c -emit-llvm -mfpmath=sse -msse -mmmx -msse2 -fpermissive -mpreferred-stack-boundary=4 -DNDEBUG -m32 -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR}/include/RTfact/AnySL)

  SET(outputFiles "")
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
  FOREACH(srcFilename ${rtfactanyslshaders})
    GET_FILENAME_COMPONENT(filename_we ${srcFilename} NAME_WE)
    SET(outputFilename ${CMAKE_CURRENT_BINARY_DIR}/shaders/${filename_we}.bc)

    FILE(TO_NATIVE_PATH ${outputFilename} outputFilename_native)
    FILE(TO_NATIVE_PATH ${srcFilename} srcFilename_native)

    SET(outputFiles ${outputFiles} ${outputFilename})
    ADD_CUSTOM_COMMAND(OUTPUT ${outputFilename} COMMAND ${LLVM_GCC} ${LLVM_CFLAGS} ${srcFilename_native} -o ${outputFilename_native})
  ENDFOREACH(srcFilename)

  IF(WIN32)
    SET(RM del)
  ELSE(WIN32)
    SET(RM rm)
  ENDIF(WIN32)

  FOREACH(slfile ${rtfactanyslrsl})
    GET_FILENAME_COMPONENT(filename_we ${slfile} NAME_WE)
    SET(rslfile ${CMAKE_CURRENT_BINARY_DIR}/shaders/rsl_${filename_we})

    FILE(TO_NATIVE_PATH ${slfile} slfile_native)
    FILE(TO_NATIVE_PATH ${rslfile} rslfile_native)

    SET(outputFiles ${outputFiles} ${rslfile}.bc)
    ADD_CUSTOM_COMMAND(OUTPUT ${rslfile}.bc COMMAND ${SDRC} -I${CMAKE_CURRENT_SOURCE_DIR}/shaders/src/rsl -o ${rslfile_native}.sdr ${slfile_native}
                                            COMMAND ${SDR2BC} -o ${rslfile_native}.bc ${rslfile_native}.sdr
                                            COMMAND ${RM} ${rslfile_native}.sdr
                                            DEPENDS ${slfile})
  ENDFOREACH(slfile)
  ADD_CUSTOM_TARGET(shaders ALL SOURCES ${outputFiles} ${rtfactanyslshaders} ${rtfactanyslrsl})

  # Gadgets

  SET(LLVM_CFLAGS -O3 -Wall -c -emit-llvm -mfpmath=sse -msse -mmmx -msse2 -fpermissive -Wno-pragmas -Wno-unknown-pragmas -fasm-blocks -DNDEBUG -m32 -D_RTFACT_EXTENSIONS -I${CMAKE_CURRENT_SOURCE_DIR}/include/RTfact/AnySL -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${Boost_INCLUDE_DIRS})

  SET(outputFiles "")
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gadgets)
  FOREACH(srcFilename ${rtfactanyslgadgets})
    GET_FILENAME_COMPONENT(filename_we ${srcFilename} NAME_WE)
    SET(outputFilename ${CMAKE_CURRENT_BINARY_DIR}/gadgets/${filename_we}.bc)

    FILE(TO_NATIVE_PATH ${outputFilename} outputFilename_native)
    FILE(TO_NATIVE_PATH ${srcFilename} srcFilename_native)

    SET(outputFiles ${outputFiles} ${outputFilename})

    ADD_CUSTOM_COMMAND(OUTPUT ${outputFilename}
                       COMMAND ${LLVM_GCC} ${LLVM_CFLAGS} ${srcFilename_native} -o ${outputFilename_native}
                       DEPENDS ${srcFilename})
  ENDFOREACH(srcFilename)
  ADD_CUSTOM_TARGET(gadgets ALL SOURCES ${outputFiles} ${rtfactanyslgadgets})

ENDIF(RTFACT_REMOTE_HAVE_ANYSL)

#@?>

# Install
###############################################################################

IF(GENERATE_INSTALL)
    install(DIRECTORY ./include
      DESTINATION .
      PATTERN ".svn" EXCLUDE
      PATTERN "*.cmake" EXCLUDE
      PATTERN "*.in" EXCLUDE)

    IF(RTFACT_REMOTE_HAVE_ANYSL)

      install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/
        DESTINATION shaders
        FILES_MATCHING PATTERN "*.bc")

      install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gadgets/
        DESTINATION bin
        FILES_MATCHING PATTERN "*.bc")

      install(FILES ${RUNTIMEBC}
        DESTINATION bin)

    ENDIF(RTFACT_REMOTE_HAVE_ANYSL)

    install(TARGETS RTfactRemote RTfactRemoteDemo RTfactRTpie RTpieDemo
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      RUNTIME DESTINATION bin)

ENDIF(GENERATE_INSTALL)

