cmake_minimum_required(VERSION 3.13.1)

set(CONF_FILE "prj.conf")

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules/libmicroxrcedds)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr_ebs)

target_sources(app PRIVATE 
    src/main.c 
    src/sensor_dds.h
    src/sensor_dds.c
    src/brake_feedback_dds.h
    src/brake_feedback_dds.c
    src/VehicleEmergencyStamped.h
    src/VehicleEmergencyStamped.c
)
