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
set(TEST_SRC_FILES ${TEST_SRC_FILES} ../net/socket/tcp_client_socket_unittest.cc)

if(WIN32)
    set(EXT_LIBS ${EXT_LIBS} ws2_32)
    add_library(icuuc SHARED IMPORTED)
    set_property(TARGET icuuc PROPERTY IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuuc54.dll)
    set_property(TARGET icuuc PROPERTY IMPORTED_IMPLIB
        ${PROJECT_SOURCE_DIR}/third_party/icu/lib/icuuc.lib)

    add_library(icuin SHARED IMPORTED)
    set_property(TARGET icuin PROPERTY IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuin54.dll)
    set_property(TARGET icuin PROPERTY IMPORTED_IMPLIB
        ${PROJECT_SOURCE_DIR}/third_party/icu/lib/icuin.lib)

    add_library(icudt SHARED IMPORTED)
    set_property(TARGET icudt PROPERTY IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icudt54.dll)
    set_property(TARGET icudt PROPERTY IMPORTED_IMPLIB
        ${PROJECT_SOURCE_DIR}/third_party/icu/lib/icudt.lib)

else(WIN32)
endif(WIN32)

set(EXT_LIBS ${EXT_LIBS} gurl icuuc icuin icudt modp_b64)

message("EXT_LIBS: ${EXT_LIBS}")

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${TEST_COMMON_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_LIB_NAME}
    ${PROJECT_IPC_LIB_NAME} ${PROJECT_NET_LIB_NAME} ${EXT_LIBS})

if(WIN32)
    # Copy to bin
    add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuuc54.dll ${CMAKE_BINARY_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icudt54.dll ${CMAKE_BINARY_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuin54.dll ${CMAKE_BINARY_DIR}/bin
        )
    # Copy to test
    add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuuc54.dll ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icudt54.dll ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third_party/icu/bin/icuin54.dll ${CMAKE_CURRENT_BINARY_DIR}
        )
endif(WIN32)

#
# ipc test
#
if(WIN32)
    set(PROJECT_IPC_TEST_NAME ${PROJECT_IPC_LIB_NAME}_ipc_tests)
    add_executable(${PROJECT_IPC_TEST_NAME} ipc_tests.cc ${TEST_COMMON_SRC_FILES})
    target_link_libraries(${PROJECT_IPC_TEST_NAME} ${PROJECT_LIB_NAME}
        ${PROJECT_IPC_LIB_NAME})
endif()

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

if(WIN32)
    target_link_libraries(${PROJECT_IPC_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})
    add_test(test1 ${PROJECT_IPC_TEST_NAME})
endif()

install (TARGETS ${PROJECT_TEST_NAME} ${PROJECT_IPC_TEST_NAME}
    DESTINATION bin)

