include(FetchContent)

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG 25f48639c7405f36766448edbf3486e81f1748e8
  GIT_SHALLOW TRUE
)

set(NLOHMANN_JSON_BUILD_MODULES ON CACHE BOOL "" FORCE)
  
FetchContent_MakeAvailable(nlohmann_json)
