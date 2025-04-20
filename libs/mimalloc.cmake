FetchContent_Declare(
    mimalloc
    GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
    GIT_TAG        v3.0.3
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
message("Fetching mimalloc")
FetchContent_MakeAvailable(mimalloc)
