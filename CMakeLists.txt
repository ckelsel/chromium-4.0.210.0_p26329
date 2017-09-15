cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME_STR chromium)
project(${PROJECT_NAME_STR} C CXX)

# LINUX and APPLE
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

find_package(Threads REQUIRED)

if(LINUX)
    INCLUDE(FindPkgConfig)
    pkg_check_modules (GTK gtk+-2.0)
    #pkg_check_modules (NSS nss)
    pkg_check_modules (NSS_UTIL nss-util)
    pkg_check_modules (GCONF gconf-2.0)
endif()

if(MSVC)
    #vc 2012 fix for vararg templates
    set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")
    set(COMMON_CXX_FLAGS "/W4  /EHsc")
    #set(COMMON_CXX_FLAGS "/W4 /WX /EHsc")
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x6000 -DUNICODE -D_UNICODE -DNOMINMAX)
else()
    set(COMMON_CXX_FLAGS "-Wall")
    #set(COMMON_CXX_FLAGS "-Wall -Werror")
    #locally defined but not used [-Wunused-local-typedefs]
    #set(COMMON_CXX_FLAGS ${COMMON_CXX_FLAGS} "-Wunused-local-typedefs")
endif() 
set(CMAKE_CXX_FLAGS ${COMMON_CXX_FLAGS})

#-------------------
# set common include folder for module
#-------------------
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR})
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

if(NOT WIN32)
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${GTK_INCLUDE_DIRS})
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${NSS_INCLUDE_DIRS})
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${NSS_UTIL_INCLUDE_DIRS})
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${GCONF_INCLUDE_DIRS})
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${PROJECT_SOURCE_DIR}/third_party/icu/public/common)
    set(COMMON_INCLUDES ${COMMON_INCLUDES} ${PROJECT_SOURCE_DIR}/third_party/icu/public/i18n)
else(NOT WIN32)
    set(COMMON_INCLUDES ${COMMON_INCLUDES}
        ${PROJECT_SOURCE_DIR}/third_party/icu/include)
endif(NOT WIN32)

#-------------------
# Module source
#-------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/googleurl)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/modp_b64)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/icu)

set(PROJECT_LIB_NAME ${PROJECT_NAME_STR}_base_impl)
add_subdirectory(${PROJECT_SOURCE_DIR}/base)

set(PROJECT_IPC_LIB_NAME ${PROJECT_NAME_STR}_ipc_impl)
add_subdirectory(${PROJECT_SOURCE_DIR}/ipc)

set(PROJECT_NET_LIB_NAME ${PROJECT_NAME_STR}_net_impl)
add_subdirectory(${PROJECT_SOURCE_DIR}/net)

#-------------------
# Test
#-------------------

if(MSVC)
    add_definitions(${MSVC_COMPILER_DEFS})
endif(MSVC)

enable_testing()

add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
add_subdirectory(${PROJECT_SOURCE_DIR}/testing)
