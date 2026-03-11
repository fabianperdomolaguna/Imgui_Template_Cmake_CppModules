find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/layers/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/python/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/visualization/*.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/**/**/*.h
)

add_executable(app
  ${SRC_FILES}
  ${HDR_FILES}
  main.cpp
)

target_include_directories(beryl PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(app
  beryl
  pybind11::embed
)

add_compile_definitions(PYTHON_VERSION="${DESIRED_PYTHON_VERSION}")

if (WIN32)
  target_link_options(app PRIVATE
    $<$<CONFIG:Release>:-Wl,/SUBSYSTEM:WINDOWS>
    $<$<CONFIG:Debug>:-Wl,/SUBSYSTEM:CONSOLE>
  )

  target_sources(app PRIVATE resources/windows_icon/app_icon.rc)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets/runtime
          $<TARGET_FILE_DIR:app>
)