set(PROJECT_LIB_NAME ${PROJECT_NAME_STR}_base_impl)

include_directories(${COMMON_INCLUDES})

set (SRC_FILES string16.cc)
set (SRC_FILES ${SRC_FILES} string_util.cc)

if(NOT WIN32)
file(GLOB SRC_FILES ${SRC_FILES} *posix.cc)
else()
file(GLOB SRC_FILES ${SRC_FILES} *win.cc)
endif()

add_library(${PROJECT_LIB_NAME} ${SRC_FILES})

