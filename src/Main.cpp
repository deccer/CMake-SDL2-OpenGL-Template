#include <SDL2/SDL.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
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
    .VerticalSyncMode = TVerticalSyncMode::VSync
};
SDL_Window* g_window = nullptr;
SDL_GLContext g_windowContext = nullptr;
ImGuiContext* g_imguiContext = nullptr;
std::chrono::time_point<std::chrono::high_resolution_clock> g_clockStart = {};

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

    g_imguiContext = ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB; // this little shit doesn't do anything
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    if (!ImGui_ImplSDL2_InitForOpenGL(g_window, g_windowContext)) {
        std::printf("ImGui: Unable to initialize the SDL2 backend\n");
        return EXIT_FAILURE;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 460")) {
        std::printf("ImGui: Unable to initialize the OpenGL backend\n");
        return EXIT_FAILURE;
    }

    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.01f, 0.02f, 0.03f, 1.0f);

    int32_t framebufferWidth = 0;
    int32_t framebufferHeight = 0;
    SDL_GL_GetDrawableSize(g_window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    using clock = std::chrono::high_resolution_clock;

    auto isFramebufferSrgbDisabled = false;
    auto isRunning = true;
    auto frameCounter = 0ull;
    auto previousTime = clock::now();

    while (isRunning) {
        ZoneScopedN("Frame");

        const auto currentTime = clock::now();
        const auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime);
        previousTime = currentTime;
        const auto framesPerSecond = std::chrono::duration<double, std::micro>(std::chrono::seconds(1)).count() / deltaTime.count();

        SDL_Event event = {};
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            // handle all window events here
        }

        if (isFramebufferSrgbDisabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
            isFramebufferSrgbDisabled = false;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Add your code here

        // Code below here is to for drawing debug ui via dear imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // start debug ui code here
        ImGui::SetNextWindowPos({32, 32});
        ImGui::SetNextWindowSize({168, 152});
        auto windowBackgroundColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        windowBackgroundColor.w = 0.4f;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBackgroundColor);
        if (ImGui::Begin("#InGameStatistics", nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoDecoration)) {

            ImGui::SeparatorText("Frame Statistics");
            ImGui::Text("afps: %f rad/s", glm::two_pi<float>() * framesPerSecond);
            ImGui::Text("dfps: %f °/s", glm::degrees(glm::two_pi<float>() * framesPerSecond));
            ImGui::TextColored(ImVec4{0.9f, 0.6f, 0.0f, 1.0f}, "rfps: %f", framesPerSecond);
            ImGui::Text("rpms: %f", framesPerSecond * 60.0f);
            ImGui::Text("  ft: %f ms", deltaTime);
            ImGui::Text("   f: %lu", frameCounter);
        }
        ImGui::End();
        ImGui::PopStyleColor();

        // end debug ui code here
        ImGui::Render();
        if (auto* imGuiDrawData = ImGui::GetDrawData(); imGuiDrawData != nullptr) {
            /*
             * next two lines are just a hack
             * https://github.com/DiligentGraphics/DiligentTools/blob/master/Imgui/src/ImGuiDiligentRenderer.cpp#L39
             * contains more information how to implement it better
             */
            glDisable(GL_FRAMEBUFFER_SRGB);
            isFramebufferSrgbDisabled = true;
            glViewport(0, 0, framebufferWidth, framebufferHeight);
            ImGui_ImplOpenGL3_RenderDrawData(imGuiDrawData);
        }

        SDL_GL_SwapWindow(g_window);
        frameCounter++;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(g_imguiContext);
    g_imguiContext = nullptr;
    SDL_GL_DeleteContext(g_windowContext);
    g_windowContext = nullptr;
    SDL_DestroyWindow(g_window);
    g_window = nullptr;
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    return EXIT_SUCCESS;
}
