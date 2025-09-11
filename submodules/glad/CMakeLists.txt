include(FetchContent)

FetchContent_Declare(glad
  URL https://github.com/Dav1dde/glad/archive/refs/tags/v2.0.8.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(glad)

string(ASCII 27 Esc)
set(Blue  "${Esc}[38;5;39m")
set(RedBright  "${Esc}[1;31m")
set(Reset  "${Esc}[0m")
set(GLAD_PYTHON_EXE "" CACHE FILEPATH "Path to Python interpreter to be used for glad (optional)")
if (GLAD_PYTHON_EXE)
  message(STATUS "${Blue}Using user-provided Python interpreter: ${GLAD_PYTHON_EXE}${Reset}")
  if(WIN32)
    set(_python_exec "${GLAD_PYTHON_EXE}/python.exe")
  else()
   set(_python_exec "${GLAD_PYTHON_EXE}/bin/python3")
  endif()
else()
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(_python_exec "${Python3_EXECUTABLE}")
  message(STATUS "${Blue}Using automatically found Python interpreter: ${_python_exec}${Reset}")
endif()

execute_process(
  COMMAND "${_python_exec}" -m pip show jinja2
  RESULT_VARIABLE JINJA2_STATUS
  OUTPUT_QUIET ERROR_QUIET
)

if (JINJA2_STATUS EQUAL 0)
  message(STATUS "${Blue}jinja2 is installed in ${_python_exec} → You can build glad successfully.${Reset}")
else()
  message(WARNING
    ${RedBright}
    "jinja2 is NOT installed in ${_python_exec}\n"
    "Please install it by running:\n"
    "  \"${_python_exec}\" -m pip install jinja2\n"
    "Or specify another interpreter with:\n"
    "  -DGLAD_PYTHON_EXE=/path/to/python/environmente"
    ${Reset}
  )
endif()


add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)

glad_add_library(glad_gl STATIC API gl:core=4.0)
