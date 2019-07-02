#pragma once

// ========================================================================== //
// Windows Headers
// ========================================================================== //

#if defined(DIB_TARGET_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <dxgi1_6.h>
#include <d3d11_4.h>
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
#include <d3d12.h>
#endif
#if defined(DIB_DEBUG)
#include <dxgidebug.h>
#endif
#endif