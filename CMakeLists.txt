cmake_minimum_required(VERSION 3.1)
set(PROJECT_NAME_STR chromium)
project(${PROJECT_NAME_STR} C CXX)

# LINUX and APPLE
if(UNIX AND NOT APPLE)
    SET(LINUX TRUE)
endif()

find_package(Threads REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
    INCLUDE(FindPkgConfig)
    pkg_check_modules (GTK gtk+-2.0)
    pkg_check_modules (NSS nss)
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
endif()

#-------------------
# Module source
#-------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/base)

#-------------------
# Test
#-------------------
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
add_definitions(${MSVC_COMPILER_DEFS})
enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_LIB_NAME}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cc)
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME_STR}_base_impl)


if(NOT WIN32)
    target_link_libraries(${PROJECT_TEST_NAME}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        )
else()
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
endif()

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})

add_test(test1 ${PROJECT_TEST_NAME})
