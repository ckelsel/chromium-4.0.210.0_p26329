
include_directories(${COMMON_INCLUDES})

set(SRC_FILES ipc_channel_proxy.cc)
set(SRC_FILES ${SRC_FILES} ipc_logging.cc)
set(SRC_FILES ${SRC_FILES} ipc_message.cc)
set(SRC_FILES ${SRC_FILES} ipc_message_utils.cc)
set(SRC_FILES ${SRC_FILES} ipc_switches.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_channel.cc)
set(SRC_FILES ${SRC_FILES} ipc_sync_message.cc)


if(UNIX)
    #set(SRC_FILES ${SRC_FILES} file_descriptor_set_posix.cc)
    set(SRC_FILES ${SRC_FILES} ipc_channel_posix.cc)
else()
    set(SRC_FILES ${SRC_FILES} ipc_channel_win.cc)
endif()

add_library(${PROJECT_IPC_LIB_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_IPC_LIB_NAME} ${EXT_LIBS})


