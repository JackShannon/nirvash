# Install script for directory: /home/colby/src/assimp/code

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "libassimp3.0-r1264")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/colby/src/assimp/lib/libassimp.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "libassimp3.0-r1264")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/colby/src/assimp/code/../include/assimp/anim.h"
    "/home/colby/src/assimp/code/../include/assimp/ai_assert.h"
    "/home/colby/src/assimp/code/../include/assimp/camera.h"
    "/home/colby/src/assimp/code/../include/assimp/color4.h"
    "/home/colby/src/assimp/code/../include/assimp/color4.inl"
    "/home/colby/src/assimp/code/../include/assimp/config.h"
    "/home/colby/src/assimp/code/../include/assimp/defs.h"
    "/home/colby/src/assimp/code/../include/assimp/cfileio.h"
    "/home/colby/src/assimp/code/../include/assimp/light.h"
    "/home/colby/src/assimp/code/../include/assimp/material.h"
    "/home/colby/src/assimp/code/../include/assimp/material.inl"
    "/home/colby/src/assimp/code/../include/assimp/matrix3x3.h"
    "/home/colby/src/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/colby/src/assimp/code/../include/assimp/matrix4x4.h"
    "/home/colby/src/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/colby/src/assimp/code/../include/assimp/mesh.h"
    "/home/colby/src/assimp/code/../include/assimp/postprocess.h"
    "/home/colby/src/assimp/code/../include/assimp/quaternion.h"
    "/home/colby/src/assimp/code/../include/assimp/quaternion.inl"
    "/home/colby/src/assimp/code/../include/assimp/scene.h"
    "/home/colby/src/assimp/code/../include/assimp/texture.h"
    "/home/colby/src/assimp/code/../include/assimp/types.h"
    "/home/colby/src/assimp/code/../include/assimp/vector2.h"
    "/home/colby/src/assimp/code/../include/assimp/vector2.inl"
    "/home/colby/src/assimp/code/../include/assimp/vector3.h"
    "/home/colby/src/assimp/code/../include/assimp/vector3.inl"
    "/home/colby/src/assimp/code/../include/assimp/version.h"
    "/home/colby/src/assimp/code/../include/assimp/cimport.h"
    "/home/colby/src/assimp/code/../include/assimp/importerdesc.h"
    "/home/colby/src/assimp/code/../include/assimp/Importer.hpp"
    "/home/colby/src/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/colby/src/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/colby/src/assimp/code/../include/assimp/IOStream.hpp"
    "/home/colby/src/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/colby/src/assimp/code/../include/assimp/Logger.hpp"
    "/home/colby/src/assimp/code/../include/assimp/LogStream.hpp"
    "/home/colby/src/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/colby/src/assimp/code/../include/assimp/cexport.h"
    "/home/colby/src/assimp/code/../include/assimp/Exporter.hpp"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/colby/src/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/colby/src/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/colby/src/assimp/code/pstdint.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")

