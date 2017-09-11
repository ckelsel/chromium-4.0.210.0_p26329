
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

#
# Common
#
set(TEST_COMMON_SRC_FILES multiprocess_func_list.cc)

#
# Unittest
#
set(PROJECT_TEST_NAME ${PROJECT_LIB_NAME}_test)

set(TEST_SRC_FILES basictypes_unittest.cc)
set(TEST_SRC_FILES ${TEST_SRC_FILES} non_thread_safe_unittest.cc)
set(TEST_SRC_FILES ${TEST_SRC_FILES} scoped_ptr_unittest.cc)
set(TEST_SRC_FILES ${TEST_SRC_FILES} simple_thread_unittest.cc)
set(TEST_SRC_FILES ${TEST_SRC_FILES} sys_info_unittest.cc)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${TEST_COMMON_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_LIB_NAME}
    ${PROJECT_IPC_LIB_NAME})

#
# ipc test
#
set(PROJECT_IPC_TEST_NAME ${PROJECT_IPC_LIB_NAME}_ipc_tests)
add_executable(${PROJECT_IPC_TEST_NAME} ipc_tests.cc ${TEST_COMMON_SRC_FILES})
target_link_libraries(${PROJECT_IPC_TEST_NAME} ${PROJECT_LIB_NAME}
    ${PROJECT_IPC_LIB_NAME})

if(NOT WIN32)
    target_link_libraries(${PROJECT_TEST_NAME}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        )

    target_link_libraries(${PROJECT_IPC_TEST_NAME}
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

    target_link_libraries(${PROJECT_IPC_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    target_link_libraries(${PROJECT_IPC_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
endif()

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})

add_test(test0 ${PROJECT_TEST_NAME})
add_test(test1 ${PROJECT_IPC_TEST_NAME})
