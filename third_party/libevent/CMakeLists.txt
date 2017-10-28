cmake_minimum_required (VERSION 2.6)
project (libevent C)

include_directories(.)
include_directories(linux)

add_definitions(-DHAVE_CONFIG_H)


set(SRC_FILES buffer.c)
set(SRC_FILES ${SRC_FILES} epoll.c)
set(SRC_FILES ${SRC_FILES} epoll_sub.c)
set(SRC_FILES ${SRC_FILES} evbuffer.c)
set(SRC_FILES ${SRC_FILES} evdns.c)
set(SRC_FILES ${SRC_FILES} event.c)
set(SRC_FILES ${SRC_FILES} event_tagging.c)
set(SRC_FILES ${SRC_FILES} evrpc.c)
set(SRC_FILES ${SRC_FILES} evutil.c)
set(SRC_FILES ${SRC_FILES} http.c)
set(SRC_FILES ${SRC_FILES} log.c)
set(SRC_FILES ${SRC_FILES} poll.c)
set(SRC_FILES ${SRC_FILES} select.c)
set(SRC_FILES ${SRC_FILES} signal.c)
set(SRC_FILES ${SRC_FILES} strlcpy.c)

add_library(libevent ${SRC_FILES})
target_link_libraries(libevent rt)
