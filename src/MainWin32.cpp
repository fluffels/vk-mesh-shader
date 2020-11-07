#pragma warning (disable: 4267)
#pragma warning (disable: 4996)

#include <iomanip>

#include <Windows.h>


#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include "DirectInput.h"
#include "Mouse.h"
#include "Render.h"
#include "RenderMesh.h"
#include "RenderText.h"
#include "State.h"
#include "Vulkan.h"
#include <vulkan/vulkan_win32.h>

using std::exception;
using std::setprecision;
using std::fixed;
using std::setw;

#define WIN32_CHECK(e, m) if (e != S_OK) throw new std::runtime_error(m)

const int WIDTH = 800;
const int HEIGHT = 800;

const float DELTA_MOVE_PER_S = 1.f;
const float MOUSE_SENSITIVITY = 0.1f;
const float JOYSTICK_SENSITIVITY = 5;

bool keyboard[VK_OEM_CLEAR] = {};

VkSurfaceKHR getSurface(
    HWND window,
    HINSTANCE instance,
    const VkInstance& vkInstance
) {
    VkSurfaceKHR surface;

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hinstance = instance;
    createInfo.hwnd = window;

    auto result = vkCreateWin32SurfaceKHR(
        vkInstance,
        &createInfo,
        nullptr,
        &surface
    );

    if (result != VK_SUCCESS) {
        throw runtime_error("could not create win32 surface");
    } else {
        return surface;
    }
}

LRESULT
WindowProc(
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) PostQuitMessage(0);
            else keyboard[(uint16_t)wParam] = true;
            break;
        case WM_KEYUP:
            keyboard[(uint16_t)wParam] = false;
            break;
    }
    return DefWindowProc(window, message, wParam, lParam);
}

int
WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCommand
) {
    LOG(INFO) << "Starting...";

    LARGE_INTEGER counterFrequency;
    QueryPerformanceFrequency(&counterFrequency);
    
    WNDCLASSEX windowClassProperties = {};
    windowClassProperties.cbSize = sizeof(windowClassProperties);
    windowClassProperties.style = CS_HREDRAW | CS_VREDRAW;
    windowClassProperties.lpfnWndProc = WindowProc;
    windowClassProperties.hInstance = instance;
    windowClassProperties.lpszClassName = "MainWindowClass";
    ATOM windowClass = RegisterClassEx(&windowClassProperties);
    if (!windowClass) {
        LOG(ERROR) << "could not create window class";
    }

    HWND window = CreateWindowEx(
        0,
        "MainWindowClass",
        "Vk Mesh Shader Example",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WIDTH,
        HEIGHT,
        NULL,
        NULL,
        instance,
        NULL
    );
    if (window == NULL) {
        LOG(ERROR) << "could not create window";
    }

    SetWindowPos(
        window,
        HWND_TOP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        SWP_FRAMECHANGED
    );
    ShowCursor(FALSE);

    Vulkan vk;
    vk.extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    createVKInstance(vk);
    vk.swap.surface = getSurface(window, instance, vk.handle);
    initVK(vk);

    Uniforms uniforms = {};
    renderInit(vk, uniforms);

    {
        FILE* save;
        auto err = fopen_s(&save, "save.dat", "r");
        if (!err) {
            fread(&uniforms.eye, sizeof(uniforms.eye), 1, save);
            fread(&uniforms.rotation, sizeof(uniforms.rotation), 1, save);
            fclose(save);
        }
    }

    DirectInput directInput(instance);
    Controller* controller = directInput.controller;
    Mouse* mouse = directInput.mouse;

    LARGE_INTEGER epoch = {};
    QueryPerformanceCounter(&epoch);

    LARGE_INTEGER frameStart = {}, frameEnd = {};
    int64_t frameDelta = {};
    float fps = 0;
    float frameTime = 0;

    int errorCode = 0;

    BOOL done = false;
    while (!done) {
        MSG msg;
        BOOL messageAvailable; 
        do {
            messageAvailable = PeekMessage(
                &msg,
                (HWND)nullptr,
                0, 0,
                PM_REMOVE
            );
            TranslateMessage(&msg); 
            if (msg.message == WM_QUIT) {
                done = true;
                errorCode = (int)msg.wParam;
            }
            DispatchMessage(&msg); 
        } while(!done && messageAvailable);

        if (!done) {
            char debugString[1024];
            snprintf(debugString, 1024, "%.2f FPS", fps);

            QueryPerformanceCounter(&frameStart);
                updateUniforms(vk, &uniforms, sizeof(uniforms));
                renderFrame(vk, debugString);
            QueryPerformanceCounter(&frameEnd);
            frameDelta = frameEnd.QuadPart - frameStart.QuadPart;
            frameTime = (float)frameDelta / counterFrequency.QuadPart;
            fps = counterFrequency.QuadPart / (float)frameDelta;

            float deltaMove = DELTA_MOVE_PER_S * frameTime;
            if (keyboard['W']) {
                eventMoveForward(deltaMove, uniforms);
            }
            if (keyboard['S']) {
                eventMoveBackward(deltaMove, uniforms);
            }
            if (keyboard['A']) {
                eventMoveLeft(deltaMove, uniforms);
            }
            if (keyboard['D']) {
                eventMoveRight(deltaMove, uniforms);
            }
            if (keyboard['F']) {
                SetWindowPos(
                    window,
                    HWND_TOP,
                    0,
                    0,
                    WIDTH,
                    HEIGHT,
                    SWP_FRAMECHANGED
                );
            }
            if (keyboard['R']) {
                eventPositionReset(uniforms);
            }

            auto mouseDelta = mouse->getDelta();
            auto mouseDeltaX = mouseDelta.x * MOUSE_SENSITIVITY;

            eventRotateY(mouseDeltaX, uniforms);

            if (controller) {
                auto state = controller->getState();

                auto joyDeltaRotateX = state.rX * JOYSTICK_SENSITIVITY;
                eventRotateY(joyDeltaRotateX, uniforms);

                auto joyDeltaMove = deltaMove * JOYSTICK_SENSITIVITY;
                auto joyDeltaMoveX = state.x * joyDeltaMove;
                auto joyDeltaMoveY = -state.y * joyDeltaMove;
                eventMoveRight(joyDeltaMoveX, uniforms);
                eventMoveForward(joyDeltaMoveY, uniforms);
            }
        }
    }

    {
        FILE* save;
        auto err = fopen_s(&save, "save.dat", "w");
        if (!err) {
            fwrite(&uniforms.eye, sizeof(uniforms.eye), 1, save);
            fwrite(&uniforms.rotation, sizeof(uniforms.rotation), 1, save);
            fclose(save);
        } else {
            LOG(ERROR) << strerror(err);
        }
    }

    return errorCode; 
}
