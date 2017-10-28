set(PROJECT_LIB_NAME ${PROJECT_NAME_STR}_base_impl)

include_directories(${COMMON_INCLUDES})

set(SRC_FILES string_util.cc)
set(SRC_FILES ${SRC_FILES} string_piece.cc)
set(SRC_FILES ${SRC_FILES} logging.cc)
#set(SRC_FILES ${SRC_FILES} debug_util.cc)

if (NOT WIN32)
set(SRC_FILES ${SRC_FILES} string16.cc)
endif()

if(NOT WIN32)
file(GLOB SRC_FILES ${SRC_FILES} *posix.cc)
else()
file(GLOB SRC_FILES ${SRC_FILES} *win.cc)
endif()

if(NOT WIN32)
    set(EXT_LIBS rt)
else()
    set(EXT_LIBS kernel32)
    SET(EXT_LIBS ${EXT_LIBS} Userenv)
    SET(EXT_LIBS ${EXT_LIBS} wtsapi32)
    SET(EXT_LIBS ${EXT_LIBS} version)
    SET(EXT_LIBS ${EXT_LIBS} advapi32)
    SET(EXT_LIBS ${EXT_LIBS} psapi)
endif()

add_library(${PROJECT_LIB_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_LIB_NAME} ${EXT_LIBS})

