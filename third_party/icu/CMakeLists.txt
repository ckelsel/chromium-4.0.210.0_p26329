cmake_minimum_required (VERSION 2.6)
project (icu C CXX ASM)

if(NOT WIN32)
    add_subdirectory(linux)
    add_subdirectory(source/common)
    add_subdirectory(source/i18n)
else(NOT WIN32)
    add_custom_command(TARGET CopyICU
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/icuuc54.dll ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/icudt54.dll ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/icuin54.dll
    ${CMAKE_BINARY_DIR}/bin)
endif(NOT WIN32)

