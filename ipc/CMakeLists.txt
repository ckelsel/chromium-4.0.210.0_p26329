
include_directories(${COMMON_INCLUDES})

set(SRC_FILES ipc_channel.h)
set(SRC_FILES ${SRC_FILES} ipc_channel_handle.h)
set(SRC_FILES ${SRC_FILES} ipc_channel_proxy.cc)
set(SRC_FILES ${SRC_FILES} ipc_channel_proxy.h)
set(SRC_FILES ${SRC_FILES} ipc_descriptors.h)
set(SRC_FILES ${SRC_FILES} ipc_logging.cc)
set(SRC_FILES ${SRC_FILES} ipc_logging.h)
set(SRC_FILES ${SRC_FILES} ipc_message.cc)
set(SRC_FILES ${SRC_FILES} ipc_message.h)
set(SRC_FILES ${SRC_FILES} ipc_message_utils.cc)
set(SRC_FILES ${SRC_FILES} ipc_message_utils.h)
set(SRC_FILES ${SRC_FILES} ipc_switches.cc)
set(SRC_FILES ${SRC_FILES} ipc_switches.h)
set(SRC_FILES ${SRC_FILES} ipc_sync_channel.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_channel.h)
set(SRC_FILES ${SRC_FILES} ipc_sync_message.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_message.h)


if(UNIX)
    set(SRC_FILES ${SRC_FILES} ipc_message_macros.h)

    set(SRC_FILES ${SRC_FILES} file_descriptor_set_posix.cc)
    set(SRC_FILES ${SRC_FILES} file_descriptor_set_posix.h)
    set(SRC_FILES ${SRC_FILES} ipc_channel_posix.cc)
    set(SRC_FILES ${SRC_FILES} ipc_channel_posix.h)
else()
    set(SRC_FILES ${SRC_FILES} ipc_channel_win.cc)
    set(SRC_FILES ${SRC_FILES} ipc_channel_win.h)
endif()

#[[
set(SRC_FILES ${SRC_FILES} file_descriptor_set_posix_unittest.cc)
set(SRC_FILES ${SRC_FILES} ipc_message_unittest.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_channel_unittest.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_message_unittest.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_message_unittest.h)

set(SRC_FILES ${SRC_FILES} ipc_fuzzing_tests.cc)
set(SRC_FILES ${SRC_FILES} ipc_send_fds_test.cc)
set(SRC_FILES ${SRC_FILES} ipc_tests.cc)
set(SRC_FILES ${SRC_FILES} ipc_tests.h)
]]

add_library(${PROJECT_IPC_LIB_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_IPC_LIB_NAME} ${EXT_LIBS})


