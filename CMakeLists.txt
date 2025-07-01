cmake_minimum_required(VERSION 3.22) # TODO: 3.24

## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(QT_CREATOR_SKIP_MAINTENANCE_TOOL_PROVIDER ON)

project(Embi-Launcher
    VERSION 0.9.0
    DESCRIPTION "Shared"
    LANGUAGES CXX)

include(Boot2Qt)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ask for all warnings and strict compliance with the standard
set(CMAKE_CXX_FLAGS "-Wall -pedantic-errors")

if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
endif()
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick)

# ─────── find_package GLOBAL ───────
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)

    set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)

else()

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickTemplates2)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickControls2)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickControls2Impl)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS VirtualKeyboard)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)

endif()

# ─────── Deploy ───────
set(DEPLOYABLE_APPS "")
list(APPEND DEPLOYABLE_APPS "00_Launcher")

set(DEPLOYABLE_APPS ${DEPLOYABLE_APPS})
# ──────────────────────

# ──── Build config ────
# add_definitions(-DREST_NO_WEBSOCKET)
# ──────────────────────

# ────── Qt stuff ──────
set(BUILD_SHARED_LIBS OFF)
qt_standard_project_setup(REQUIRES 6.8)
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0003 NEW)
qt_policy(SET QTP0004 NEW)
qt_policy(SET QTP0005 NEW)

set(CMAKE_OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/../${CMAKE_PROJECT_NAME}")
    
if(BOOT2QT)

    # optimization
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_OUTPUT_PREFIX}_Boot2Qt_${QT_VERSION}/v${CMAKE_PROJECT_VERSION} (${CMAKE_PROJECT_DESCRIPTION})")
    set(QT_DEPLOY_PREFIX "/Ecosysteme")

    add_definitions(-DQ_OS_BOOT2QT)

    if(RASPBERRY_PI4)
        add_definitions(-DQ_DEVICE_RASPBERRY_PI4)
    endif()

    if(APALIS_IMX8)
        add_definitions(-DQ_DEVICE_APALIS_IMX8)
        add_definitions(-DQ_MANUAL_CORE_AFFINITY)
    endif()

    message("QT_DEPLOY_PREFIX = ${QT_DEPLOY_PREFIX}")
    message("QT_DEPLOY_BIN_DIR = ${QT_DEPLOY_BIN_DIR}")
    message("QT_DEPLOY_SUPPORT = ${QT_DEPLOY_SUPPORT}")
    message("QT_DEPLOY_QML_DIR = ${QT_DEPLOY_QML_DIR}")

else()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_OUTPUT_PREFIX}")

endif()
# ──────────────────────

# ───── QMLCACHEGEN ────
set(QMLCACHEGEN_STATIC ON)
set(QMLCACHEGEN_DIRECT OFF)
set(QMLCACHEGEN_VERBOSE ON)
set(QMLCACHEGEN_BYTECODE OFF)
# ──────────────────────

# ─────── QMLLINT ──────
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qt/qml")
set(QT_QML_GENERATE_QMLLS_INI ON)
# ──────────────────────

# ───── QT CREATOR ─────
set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qt/qml" CACHE STRING "Qt Creator extra qml import paths" FORCE)
# ──────────────────────

include(GitTag)
string(TIMESTAMP BUILD_DATE)

add_definitions(
    -DECOSYSTEM_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
    -DECOSYSTEM_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
    -DECOSYSTEM_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}
    -DECOSYSTEM_DESCRIPTION="${CMAKE_PROJECT_DESCRIPTION}"
    -DECOSYSTEM_PROJECT_NAME="${CMAKE_PROJECT_NAME}"
    -DECOSYSTEM_COMPANY="Embi OS"
    -DECOSYSTEM_WEBSITE="https://github.com/Embi-OS"
    -DECOSYSTEM_DOMAIN="embi.os"
    -DECOSYSTEM_COPYRIGHT="Copyright © by Embi-OS"
)

add_subdirectory(libraries)
add_subdirectory(applications)

# ─────── VERBOSE ──────
message(STATUS "[PROJECT] QT_QML_OUTPUT_DIRECTORY           = ${QT_QML_OUTPUT_DIRECTORY}")
message(STATUS "[PROJECT] QML_IMPORT_PATH                   = ${QML_IMPORT_PATH}")
message(STATUS "[PROJECT] CMAKE_PROJECT_NAME                = ${CMAKE_PROJECT_NAME}")
message(STATUS "[PROJECT] CMAKE_RUNTIME_OUTPUT_DIRECTORY    = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "[PROJECT] CMAKE_VERSION                     = ${CMAKE_VERSION}")
message(STATUS "[PROJECT] CMAKE_TOOLCHAIN_FILE              = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] CMAKE_PREFIX_PATH                 = ${CMAKE_PREFIX_PATH}")
message(STATUS "[PROJECT] CMAKE_BUILD_TYPE                  = ${CMAKE_BUILD_TYPE}")
message(STATUS "[PROJECT] QT_QML_DEBUG                      = ${QT_QML_DEBUG}")
message(STATUS "[PROJECT] SOURCE_DIR                        = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[PROJECT] BIN_DIR                           = ${CMAKE_BINARY_DIR}")
message(STATUS "[PROJECT] QT_VERSION                        = ${QT_VERSION}")
message(STATUS "[PROJECT] DEPLOYABLE_APPS                   = ${DEPLOYABLE_APPS}")

message(STATUS "[DEFINE] ECOSYSTEM_VERSION             = ${CMAKE_PROJECT_VERSION}")
message(STATUS "[DEFINE] ECOSYSTEM_DESCRIPTION         = ${CMAKE_PROJECT_DESCRIPTION}")
message(STATUS "[DEFINE] ECOSYSTEM_BUILD_DATE          = ${BUILD_DATE}")
message(STATUS "[DEFINE] ECOSYSTEM_GIT_TAG_HEX         = ${GIT_TAG_HEX}")

message(STATUS "[DEFINE] BOOT2QT                    = ${BOOT2QT}")
message(STATUS "[DEFINE] RASPBERRY_PI4              = ${RASPBERRY_PI4}")
message(STATUS "[DEFINE] APALIS_IMX8                = ${APALIS_IMX8}")
# ──────────────────────

message("CMAKE_MAKE_PROGRAM = ${CMAKE_MAKE_PROGRAM}")
message("CMAKE_COMMAND = ${CMAKE_COMMAND}")
message("CMAKE_ROOT = ${CMAKE_ROOT}")
message("CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
message("CMAKE_VS_PLATFORM_NAME = ${CMAKE_VS_PLATFORM_NAME}")
message("CMAKE_SYSTEM = ${CMAKE_SYSTEM}")
message("CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_SYSTEM_VERSION = ${CMAKE_SYSTEM_VERSION}")
message("CMAKE_HOST_SYSTEM = ${CMAKE_HOST_SYSTEM}")
message("CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM_VERSION = ${CMAKE_HOST_SYSTEM_VERSION}")
message("CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message("QML_DEBUG_FLAG = ${QML_DEBUG_FLAG}")
message("SYSTEM = ${SYSTEM}")
