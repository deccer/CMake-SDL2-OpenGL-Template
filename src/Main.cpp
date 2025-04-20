#include <SDL2/SDL.h>
#include <tracy/TracyOpenGL.hpp>

enum class TWindowStyle {
    Windowed,
    Fullscreen,
    FullscreenExclusive
};

enum class TVerticalSyncMode {
    Adaptive,
    VSync,
    Off
};

struct TSettings {
    int32_t ResolutionWidth;
    int32_t ResolutionHeight;
    float ResolutionScale;
    TWindowStyle WindowStyle;
    bool IsDebug;
    TVerticalSyncMode VerticalSyncMode;
};

TSettings g_applicationSettings = {
    .ResolutionWidth = 1680,
    .ResolutionHeight = 1050,
    .ResolutionScale = 1.0f,
    .IsDebug = true,
    .VerticalSyncMode = TVerticalSyncMode::Adaptive
};
SDL_Window* g_window = nullptr;
SDL_GLContext g_windowContext = nullptr;

auto main(
    [[maybe_unused]] int32_t argc,
    [[maybe_unused]] char* argv[]) -> int32_t {

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    auto windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    const auto isWindowWindowed = g_applicationSettings.WindowStyle == TWindowStyle::Windowed;
    if (isWindowWindowed) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    } else {
        windowFlags |= g_applicationSettings.WindowStyle == TWindowStyle::FullscreenExclusive
            ? SDL_WINDOW_FULLSCREEN
            : SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    const auto windowWidth = g_applicationSettings.ResolutionWidth;
    const auto windowHeight = g_applicationSettings.ResolutionHeight;
    const auto windowLeft = isWindowWindowed ? SDL_WINDOWPOS_CENTERED : 0u;
    const auto windowTop = isWindowWindowed ? SDL_WINDOWPOS_CENTERED : 0u;

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_RELEASE_BEHAVIOR, SDL_GLcontextReleaseFlag::SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH);
    auto contextFlags = SDL_GLcontextFlag::SDL_GL_CONTEXT_RESET_ISOLATION_FLAG | SDL_GLcontextFlag::SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
    if (g_applicationSettings.IsDebug) {
        contextFlags |= SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG;
    }
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, contextFlags);

    g_window = SDL_CreateWindow(
        "Project",
        windowLeft,
        windowTop,
        windowWidth,
        windowHeight,
        windowFlags);
    if (g_window == nullptr) {
        std::printf("Unable to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    g_windowContext = SDL_GL_CreateContext(g_window);
    if (g_windowContext == nullptr) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
        SDL_Quit();
        std::printf("Unable to create opengl context %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_GL_MakeCurrent(g_window, g_windowContext);
    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == GL_FALSE) {
        std::printf("Unable to load opengl functions\n");
        return EXIT_FAILURE;
    }

    if (g_applicationSettings.VerticalSyncMode == TVerticalSyncMode::Adaptive) {
        const auto swapInterval = SDL_GL_SetSwapInterval(-1);
        if (swapInterval == 0) {
            std::printf("VSync: Adaptive\n");
        } else if (swapInterval < 0) {
            std::printf("VSync: Adaptive not supported, enabled ordinary vsync. %s\n", SDL_GetError());
            SDL_GL_SetSwapInterval(1);
        }
    } else {
        const auto isVSync = g_applicationSettings.VerticalSyncMode == TVerticalSyncMode::VSync;
        SDL_GL_SetSwapInterval(isVSync ? 1 : 0);
        std::printf("VSync: %s\n", isVSync ? "On" : "Off");
    }

    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.01f, 0.02f, 0.03f, 1.0f);

    auto isRunning = true;

    while (isRunning) {
        ZoneScopedN("Frame");

        SDL_Event event = {};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(g_window);
    }

    SDL_GL_DeleteContext(g_windowContext);
    g_windowContext = nullptr;
    SDL_DestroyWindow(g_window);
    g_window = nullptr;
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    return EXIT_SUCCESS;
}
