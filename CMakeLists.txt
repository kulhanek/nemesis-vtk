# ==============================================================================
# Nemesis CMake File
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9 FATAL_ERROR)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

# define project name ----------------------------------------------------------
SET(NEMESIS_VERSION 15.x)

# what languages we use --------------------------------------------------------
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)
IF(WIN32)
    SET(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
ENDIF(WIN32)

# external dependencies --------------------------------------------------------
IF(NOT DEFINED COMPOSITE_PROJECT)
    SET(DEVELOPMENT_ROOT "$ENV{DEVELOPMENT_ROOT}")
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

IF(NOT DEVELOPMENT_ROOT)
    MESSAGE(FATAL_ERROR "DEVELOPMENT_ROOT variable is not set!")
ENDIF(NOT DEVELOPMENT_ROOT)

# ==============================================================================
# external libraries -----------------------------------------------------------
# ==============================================================================

# HIPOLY -----------------------------------------
SET(HIPOLY_ROOT ${DEVELOPMENT_ROOT}/projects/hipoly/1.0)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/compress SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/color SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/error SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/event SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/file SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/list SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/option SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/string SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/thread SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/time SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/xml SYSTEM)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/stream SYSTEM)
LINK_DIRECTORIES(${HIPOLY_ROOT}/lib)
SET(HIPOLY_LIB_NAME hipoly)

# SCIMAFIC ---------------------------------------
SET(SCIMAFIC_ROOT ${DEVELOPMENT_ROOT}/projects/scimafic/1.0)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic SYSTEM)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/maths SYSTEM)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/files SYSTEM)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/chemistry SYSTEM)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/physics SYSTEM)
LINK_DIRECTORIES(${SCIMAFIC_ROOT}/lib)
SET(SCIMAFIC_CLIB_NAME cscimafic)

# ASL --------------------------------------------
SET(ASL_ROOT ${DEVELOPMENT_ROOT}/projects/asl/2.0)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl SYSTEM)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/mask SYSTEM)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/restart SYSTEM)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/topology SYSTEM)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/trajectory SYSTEM)
LINK_DIRECTORIES(${ASL_ROOT}/lib)
SET(ASL_LIB_NAME asl)

ADD_DEFINITIONS(-DUNIX)

# FIREBIRD ---------------------------------------
#SET(FBCLIENT_ROOT ${DEVELOPMENT_ROOT}/thirdparty/fbclient/2.5.2.26539)
#INCLUDE_DIRECTORIES(${FBCLIENT_ROOT}/include SYSTEM)
#LINK_DIRECTORIES(${FBCLIENT_ROOT}/lib)
#SET(FBCLIENT_ROOT fbclient)

# FIREBIRD ---------------------------------------
#SET(FIREBIRD_ROOT ${DEVELOPMENT_ROOT}/projects/firebird/1.0)
#INCLUDE_DIRECTORIES(${FIREBIRD_ROOT}/src/lib/firebird SYSTEM)
#LINK_DIRECTORIES(${FIREBIRD_ROOT}/lib)
#SET(FIREBIRD_LIB_NAME firebird)

# OpenBabel --------------------------------------
SET(OPEN_BABEL_ROOT ${DEVELOPMENT_ROOT}/thirdparty/obcore/2.3.1)
INCLUDE_DIRECTORIES(${OPEN_BABEL_ROOT}/include)
LINK_DIRECTORIES(${OPEN_BABEL_ROOT}/lib)
SET(OPEN_BABEL_LIB obcore)

# Indigo -----------------------------------------
SET(INDIGO_ROOT ${DEVELOPMENT_ROOT}/thirdparty/indigo/1.1)
INCLUDE_DIRECTORIES(${INDIGO_ROOT}/src/api)
LINK_DIRECTORIES(${INDIGO_ROOT}/lib)
SET(INDIGO_LIB indigo)

# FreeType ----------------------------------------
SET(FT2_ROOT ${DEVELOPMENT_ROOT}/thirdparty/freetype/2.4.10)
INCLUDE_DIRECTORIES(${FT2_ROOT}/include)
LINK_DIRECTORIES(${FT2_ROOT}/lib)
SET(FT2_LIB freetype)

# FTGL -------------------------------------------
SET(FTGL_ROOT ${DEVELOPMENT_ROOT}/thirdparty/ftgl/2.1.3rc5)
INCLUDE_DIRECTORIES(${FTGL_ROOT}/src/lib/ftgl)
LINK_DIRECTORIES(${FTGL_ROOT}/lib)
SET(FTGL_LIB ftgl)

# GLEW -------------------------------------------
SET(GLEW_ROOT ${DEVELOPMENT_ROOT}/thirdparty/glew/1.5.8/)
INCLUDE_DIRECTORIES(${GLEW_ROOT}/include)
LINK_DIRECTORIES(${GLEW_ROOT}/lib)
SET(GLEW_LIB glew)

# GLS -------------------------------------------
SET(GLS_ROOT ${DEVELOPMENT_ROOT}/thirdparty/gls/1.0.0/)
INCLUDE_DIRECTORIES(${GLS_ROOT}/include)
LINK_DIRECTORIES(${GLS_ROOT}/lib)
SET(GLS_LIB gls)

# ActiveMQ-CPP -----------------------------------
#SET(CMSCORE_ROOT ${DEVELOPMENT_ROOT}/thirdparty/cmscore/3.4.1)
#INCLUDE_DIRECTORIES(${CMSCORE_ROOT}/src/lib/cmscore/cmscore)
#LINK_DIRECTORIES(${CMSCORE_ROOT}/lib)
#SET(CMSCORE_LIB cmscore)

# boost ---------------------------------------
SET(BOOST_ROOT ${DEVELOPMENT_ROOT}/thirdparty/boost/1.59.0)
INCLUDE_DIRECTORIES(${BOOST_ROOT} SYSTEM)

# nLEaP ------------------------------------------
SET(NLEAP_ROOT ${DEVELOPMENT_ROOT}/projects/nleap/1.0)
INCLUDE_DIRECTORIES(${NLEAP_ROOT}/src/lib/nleap)
INCLUDE_DIRECTORIES(${NLEAP_ROOT}/src/lib/nleapcmds)
LINK_DIRECTORIES(${NLEAP_ROOT}/lib)
SET(NLEAP_LIB nleap)
SET(NLEAP_CMDS_LIB nleapcmds)

# OpenGL -----------------------------------------
FIND_PACKAGE(OpenGL REQUIRED)

# QT5 --------------------------------------------
SET(CMAKE_AUTOMOC ON)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Script REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)
SET(HAVE_QT5 1)
ADD_DEFINITIONS(-DHAVE_QT5)

# policies ---------------------------------------
IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0020 NEW)
ENDIF(COMMAND cmake_policy)

# ==============================================================================
# output directories -----------------------------------------------------------
# ==============================================================================

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

# ==============================================================================
# rpaths -----------------------------------------------------------------------
# ==============================================================================

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH
        "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/openbabel/2.3.0")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# ==============================================================================
# compiler setups --------------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wundef -Wno-long-long")

    #SET(CMAKE_BUILD_TYPE Release)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

# ==============================================================================
# project subdirectories  ------------------------------------------------------
# ==============================================================================
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(etc)
ADD_SUBDIRECTORY(share)


