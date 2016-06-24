cmake_minimum_required(VERSION 2.8.3)
project(file_descriptor_activity_test)

# Defines the orocos_* cmake macros. See that file for additional
# documentation.
find_package(OROCOS-RTT REQUIRED)
include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake)

orocos_component(${PROJECT_NAME} src/file_descriptor_activity_test.cpp)
