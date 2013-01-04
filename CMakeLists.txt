cmake_minimum_required (VERSION 2.8)
project (passthruapp)

set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_SOURCE_DIR})
set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_INC_DIRS} PARENT_SCOPE)

message(STATUS "Generating ${PROJECT_NAME}")
