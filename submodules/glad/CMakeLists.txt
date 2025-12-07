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

set(PYTHON_GLAD "" CACHE STRING "Custom Python interpreter for Glad")

if(PYTHON_GLAD)
  if (NOT EXISTS "${PYTHON_GLAD}")
    message(FATAL_ERROR "${RedBright}El Python especificado no existe: ${PYTHON_GLAD}${Reset}")
  endif()
  set(GLAD_PYTHON "${PYTHON_GLAD}" CACHE STRING "Python interpreter used by glad" FORCE)
  message(STATUS "${Blue}Glad: using specified Python: ${GLAD_PYTHON}${Reset}")
else()  
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  set(Python_EXECUTABLE "${Python3_EXECUTABLE}" CACHE STRING "" FORCE)
  set(Python_ROOT_DIR "${Python3_EXECUTABLE}" CACHE STRING "" FORCE)
  set(GLAD_PYTHON "${Python3_EXECUTABLE}" CACHE STRING "Python interpreter used by glad" FORCE)
  message(STATUS "${Blue}Glad: using system Python: ${GLAD_PYTHON}${Reset}")
endif()

if(WIN32)
  if(GLAD_PYTHON MATCHES "python.exe$")
    set(GLAD_PY "${GLAD_PYTHON}")
  else()
    set(GLAD_PY "${GLAD_PYTHON}\\python.exe")
  endif()
else()
  if(EXISTS "${GLAD_PYTHON}" AND NOT IS_DIRECTORY "${GLAD_PYTHON}")
    set(GLAD_PY "${GLAD_PYTHON}")
  else()
    set(GLAD_PY "${GLAD_PYTHON}/bin/python")
  endif()
endif()

execute_process(
  COMMAND "${GLAD_PY}" -m pip show jinja2
  RESULT_VARIABLE _JINJA2
  OUTPUT_QUIET ERROR_QUIET
)

if (NOT _JINJA2 EQUAL 0)
  message(WARNING
    "${RedBright}jinja2 is NOT installed in: ${GLAD_PYTHON}.
    Install it with:
      \"${GLAD_PY}\" -m pip install jinja2${Reset}"
  )
endif()

set(Python_FIND_QUIETLY TRUE)
add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)

glad_add_library(glad_gl SHARED API gl:core=4.0)
