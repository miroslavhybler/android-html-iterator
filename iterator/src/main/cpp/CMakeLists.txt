cmake_minimum_required(VERSION 3.22.1)

project("html-iterator")

if (DEFINED IS_LOGGING_ENABLED)
    add_definitions(-DIS_LOGGING_ENABLED=1)
else()
    add_definitions(-DIS_LOGGING_ENABLED=0)
endif()

add_library(
        ${CMAKE_PROJECT_NAME} SHARED
        HtmlIterator.h
        HtmlIteratorCallback.h
        HtmlUtils.h
        PlatformUtils.h
        ITERATOR_JNI.cpp
)

target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        android
        log
)