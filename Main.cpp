#include "Main.h"
#include "icons.h"
#include "mapper/kdmapper.hpp"

void color()
{
    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_TitleBg] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.40f, 0.400f, 0.40f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
    colors[ImGuiCol_Button] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImColor(144, 200, 144);
    colors[ImGuiCol_ButtonActive] = ImColor(144, 217, 144);
    colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.57f);
    colors[ImGuiCol_TabActive] = ImColor(144, 220, 144);
    colors[ImGuiCol_Tab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
}

static bool showing = true;
static int currentColumn = 0;
static int currentPage = 0;
using namespace KeyAuth;

char user[255] = "";
char pass[255] = "";
char key[255] = "";

// Main code
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // watch video if you need help understanding how to input appliction credentials https://www.youtube.com/watch?v=uJ0Umy_C6Fg

    std::string name = XorStr("");
    std::string ownerid = XorStr("");
    std::string secret = XorStr("");
    std::string version = XorStr("1.0");

    api KeyAuthApp(name, ownerid, secret, version);

    KeyAuthApp.init();

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = true;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF,
        0x0400, 0x044F,
        0,
    };



    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = 3;
    icons_config.OversampleV = 3;

    ImFontConfig CustomFont;
    CustomFont.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 14.0f, &CustomFont);

    io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 12.0f, &icons_config, icons_ranges);
    io.Fonts->AddFontDefault();

    DWORD window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    auto x = float(screen_rect.right - WINDOW_WIDTH) / 2.f;
    auto y = float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowBgAlpha(1.0f);

            if (GetAsyncKeyState(VK_INSERT) & 1)
            {
                showing = !showing;
            }
            if (showing)
            {
                ImGui::Begin(LOADER_BRAND, &loader_active, window_flags);
                {

                    color();

                   

                    switch (currentColumn) {
                        break;
                    case 0:
                        if (ImGui::BeginTabBar("Auth"))
                        {
                            
                            ImGui::Text("Login Page:");
                            ImGui::InputText("Username", user, IM_ARRAYSIZE(user));
                            ImGui::InputText("Password", pass, IM_ARRAYSIZE(pass), ImGuiInputTextFlags_Password);
                            if (ImGui::Button("Login", ImVec2(103,20)))
                            {
                                if (KeyAuthApp.login(user, pass)) currentColumn = 1;
                            }
                          //  ImGui::SameLine();
                            ImGui::SetCursorPos(ImVec2(8, 24));
                            if (ImGui::Button(ICON_FA_USERS" | Register Page", ImVec2(103, 20)))
                            {
                                currentColumn = 3;
                            }
                            ImGui::EndTabBar();
                        }
                        break;
                    case 3:
                        if (ImGui::BeginTabBar("Auth"))
                        {
                            ImGui::Text("Register Page:");
                            ImGui::InputText("Username", user, IM_ARRAYSIZE(user));
                            ImGui::InputText("Password", pass, IM_ARRAYSIZE(pass), ImGuiInputTextFlags_Password);
                            ImGui::InputText("Key", key, IM_ARRAYSIZE(key), ImGuiInputTextFlags_Password);
                            if (ImGui::Button("Register", ImVec2(103, 20)))
                            {
                                if (KeyAuthApp.regstr(user, pass, key)) currentColumn = 2;
                            }
                          //  ImGui::SameLine();
                            ImGui::SetCursorPos(ImVec2(8, 24));
                            if (ImGui::Button(ICON_FA_USER" | Login Page", ImVec2(103, 20)))
                            {
                                currentColumn = 0;
                            }
                            ImGui::EndTabBar();
                        }
                        break;
                    case 2:
                        ImGui::Text("Thank You For Registering:");
                        ImGui::SameLine();
                        ImGui::Text(user);
                        ImGui::Text("Goto main here ->");
                        ImGui::SameLine();
                        if (ImGui::Button("Main Page"))
                        {
                            currentColumn = 1;
                        }
                        break;
                    case 1:
                        //if (ImGui::BeginTabBar("Your Code Here"))
                       // {
                        ImGui::SetCursorPos(ImVec2(140, 130));

                            if (ImGui::Button("Spoof", ImVec2(103,20))) {
                                BlockInput(true);
                                SetCursorPos(1200, 1200);
                                std::vector<std::uint8_t> driver = KeyAuthApp.download("fileidhere"); // upload your file to KeyAuth then put driver file ID here

                                HANDLE iqvw64e_device_handle = intel_driver::Load();

                                if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
                                {
                                    std::cout << XorStr("\n Error creating the driver, ensure Vanguard anticheat isn't running and all antivirus products are disabled and try again.");
                                    Sleep(3500);
                                    exit(0);
                                }

                                if (!kdmapper::MapDriver(iqvw64e_device_handle, driver.data()))
                                {
                                    std::cout << XorStr("\n Error creating the driver, ensure Vanguard anticheat isn't running and all antivirus products are disabled and try again.");
                                    intel_driver::Unload(iqvw64e_device_handle);
                                    Sleep(3500);
                                    exit(0);
                                }

                                intel_driver::Unload(iqvw64e_device_handle);
                                BlockInput(false);
                            }
                           // ImGui::EndTabBar();
                       // }
                        break;
                    }
                }
            }
            ImGui::End();
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}