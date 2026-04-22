cmake_minimum_required(VERSION 3.16)
project(rmcs_laser_guidance)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-O2 -Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(hikcamera REQUIRED)
find_package(yaml-cpp REQUIRED)

include(GNUInstallDirs)

set(default_config_path "${CMAKE_CURRENT_SOURCE_DIR}/configs/default.yaml")

add_library(
  ${PROJECT_NAME}_core
  SHARED
  src/config.cpp
  src/pipeline.cpp
)

target_include_directories(
  ${PROJECT_NAME}_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}_core PUBLIC yaml-cpp ${OpenCV_LIBS})

add_executable(hikcamera_preview apps/hikcamera_preview.cpp)
target_compile_definitions(
  hikcamera_preview
  PRIVATE RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH="${default_config_path}"
)
target_include_directories(hikcamera_preview PRIVATE ${hikcamera_INCLUDE_DIRS})
target_link_libraries(
  hikcamera_preview
  PRIVATE
    ${PROJECT_NAME}_core
    ${hikcamera_LIBRARIES}
    ${OpenCV_LIBS}
)

add_executable(offline_smoke apps/offline_smoke.cpp)
target_compile_definitions(
  offline_smoke
  PRIVATE RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH="${default_config_path}"
)
target_link_libraries(offline_smoke PRIVATE ${PROJECT_NAME}_core ${OpenCV_LIBS})

add_executable(pipeline_smoke_test tests/pipeline_smoke_test.cpp)
target_compile_definitions(
  pipeline_smoke_test
  PRIVATE RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH="${default_config_path}"
)
target_link_libraries(pipeline_smoke_test PRIVATE ${PROJECT_NAME}_core ${OpenCV_LIBS})

enable_testing()
add_test(NAME pipeline_smoke_test COMMAND pipeline_smoke_test)

install(
  TARGETS ${PROJECT_NAME}_core hikcamera_preview offline_smoke pipeline_smoke_test
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY configs
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY docs
  DESTINATION share/${PROJECT_NAME}
)

install(
  FILES README.md AGENTS.md plan.md
  DESTINATION share/${PROJECT_NAME}
)

ament_export_include_directories(${CMAKE_INSTALL_INCLUDEDIR})
ament_export_dependencies(Eigen3 OpenCV hikcamera yaml-cpp)

ament_package()
