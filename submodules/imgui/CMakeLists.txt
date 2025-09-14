include(FetchContent)

FetchContent_Declare(imgui
   GIT_REPOSITORY https://github.com/ocornut/imgui.git
   GIT_TAG        v1.92.1-docking
   GIT_SHALLOW    TRUE
)

FetchContent_GetProperties(imgui)


FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
   ${imgui_SOURCE_DIR}/imgui.cpp
   ${imgui_SOURCE_DIR}/imgui_draw.cpp
   ${imgui_SOURCE_DIR}/imgui_tables.cpp
   ${imgui_SOURCE_DIR}/imgui_widgets.cpp
   ${imgui_SOURCE_DIR}/imgui_demo.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
   $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
   $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
)

target_link_libraries(imgui
   glfw
)