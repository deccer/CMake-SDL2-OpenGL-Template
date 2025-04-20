add_custom_target(CopyData ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)

add_executable(Project
    Main.cpp
)
add_dependencies(Project
    CopyData
)

target_compile_definitions(Project
    PRIVATE GLM_FORCE_CXX2A
    PRIVATE GLM_FORCE_SILENT_WARNINGS
    PRIVATE GLM_ENABLE_EXPERIMENTAL

    PRIVATE GLM_FORCE_RADIANS
    PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE
    PRIVATE GLM_FORCE_RIGHT_HANDED
    PRIVATE GLM_FORCE_SWIZZLE
    PRIVATE GLM_FORCE_XYZW_ONLY
)

target_precompile_headers(Project
    PUBLIC <array>
    PUBLIC <chrono>
    PUBLIC <cstdint>
    #PUBLIC <entt/entity/registry.hpp>
    #PUBLIC <entt/entity/entity.hpp>
    PUBLIC <expected>
    PUBLIC <filesystem>
    PUBLIC <glad/gl.h>
    PUBLIC <glm/gtc/quaternion.hpp>
    PUBLIC <glm/gtc/type_ptr.hpp>
    PUBLIC <glm/mat4x4.hpp>
    PUBLIC <glm/vec2.hpp>
    PUBLIC <glm/vec3.hpp>
    PUBLIC <glm/vec4.hpp>
    PUBLIC <iomanip>
    PUBLIC <memory>
    PUBLIC <optional>
    #PUBLIC <parallel_hashmap/phmap.h>
    PUBLIC <ranges>
    PUBLIC <sstream>
    PUBLIC <string>
    PUBLIC <string_view>
    PUBLIC <utility>
    PUBLIC <vector>
)

target_link_libraries(Project
    PRIVATE debugbreak
    PRIVATE SDL2::SDL2-static
    PRIVATE glad
    PRIVATE glm
    #PRIVATE imgui
    #PRIVATE imguizmo
    #PRIVATE stb
    #PRIVATE poolSTL::poolSTL
    #PRIVATE phmap
    #PRIVATE tbb
    #PRIVATE EnTT
    #PRIVATE fastgltf::fastgltf
    #PRIVATE mimalloc-static
    #PRIVATE Jolt
    #PRIVATE mikktspace
    #PRIVATE soloud
    #PRIVATE TracyClient
)
