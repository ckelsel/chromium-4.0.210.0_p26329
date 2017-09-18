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
    set(EXT_LIBS ${EXT_LIBS} libevent pthread gthread-2.0)
endif(WIN32)

set(EXT_LIBS ${EXT_LIBS} gurl icuuc icuin icudt modp_b64)

message("EXT_LIBS: ${EXT_LIBS}")

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${TEST_COMMON_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_LIB_NAME}
    ${PROJECT_IPC_LIB_NAME} ${PROJECT_NET_LIB_NAME} ${EXT_LIBS})

if(WIN32)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    
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

#
# net test
#
set(PROJECT_NET_TEST_SRCS ../net/base/address_list_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/run_all_unittests.cc)
#[[
#set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/base64_unittest.cc)
#set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/bzip2_filter_unittest.cc)
#set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/cookie_monster_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/cookie_policy_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/data_url_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/directory_lister_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/dns_util_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/escape_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/file_stream_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/filter_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/gzip_filter_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/host_cache_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/host_resolver_impl_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/load_log_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/load_log_util_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/listen_socket_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/mime_sniffer_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/mime_util_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/net_util_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/registry_controlled_domain_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/sdch_filter_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/ssl_client_auth_cache_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/strict_transport_security_state_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/telnet_server_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/test_completion_callback_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/x509_certificate_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/addr_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/backend_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/bitmap_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/block_files_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/disk_cache_test_base.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/entry_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/mapped_file_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/disk_cache/storage_block_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/ftp/ftp_auth_cache_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/ftp/ftp_ctrl_response_buffer_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/ftp/ftp_network_transaction_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/des_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_auth_cache_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_auth_handler_basic_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_auth_handler_digest_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_auth_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_byte_range_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_cache_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_chunked_decoder_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_network_layer_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_network_transaction_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_response_headers_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_transaction_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_util_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/http/http_vary_data_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/init_proxy_resolver_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_config_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_list_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_resolver_js_bindings_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_resolver_v8_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_script_fetcher_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_server_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_service_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/single_threaded_proxy_resolver_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/client_socket_pool_base_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/socks5_client_socket_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/socks_client_socket_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/ssl_client_socket_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/tcp_client_socket_pool_unittest.cc)
]]
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/tcp_client_socket_unittest.cc)
set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/socket/tcp_pinger_unittest.cc)
#set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/url_request/url_request_unittest.cc)
if(WIN32)
    #set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/ssl_config_service_win_unittest.cc)
    #set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/base/wininet_util_unittest.cc)
    #set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_config_service_win_unittest.cc)
else(WIN32)
    #set(PROJECT_NET_TEST_SRCS ${PROJECT_NET_TEST_SRCS} ../net/proxy/proxy_config_service_linux_unittest.cc)
endif(WIN32)

if (WIN32)
    add_definitions(/D_WINSOCKAPI_)
endif (WIN32)

set(PROJECT_NET_TEST_NAME ${PROJECT_NET_LIB_NAME}_net_tests)
add_executable(${PROJECT_NET_TEST_NAME} ${PROJECT_NET_TEST_SRCS}
    ${TEST_COMMON_SRC_FILES})
target_link_libraries(${PROJECT_NET_TEST_NAME} ${PROJECT_LIB_NAME}
    ${PROJECT_NET_LIB_NAME} ${EXT_LIBS})

if(NOT WIN32)
    target_link_libraries(${PROJECT_TEST_NAME}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        )

    target_link_libraries(${PROJECT_NET_TEST_NAME}
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

    target_link_libraries(${PROJECT_NET_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    target_link_libraries(${PROJECT_NET_TEST_NAME}
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

target_link_libraries(${PROJECT_NET_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})
add_test(test1 ${PROJECT_NET_TEST_NAME})

install(TARGETS ${PROJECT_TEST_NAME} ${PROJECT_IPC_TEST_NAME} ${PROJECT_NET_TEST_NAME}
    DESTINATION bin)

