include(FetchContent)

FetchContent_Declare(nlohmann_json
   URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
   DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
  
FetchContent_MakeAvailable(nlohmann_json)
