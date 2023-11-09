#pragma once

#include <Windows.h>
#include <thread>
#include <d3d9.h>
#include <vector>
#include <string>
#include <TlHelp32.h>
#include <sstream>

//our files
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

#include "IconFontAwesome6.h"
#include "IconFontAwesome6_Brands.h"

#include "utils.hpp"


extern ImFont* pFontAwesomeSolid, *pFontAwesomeReg, * pFontAwesomeBrands, * pFontArial;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);


namespace Render
{


	int WIDTH = 400;
	int HEIGHT = 270;

	bool active = true;

	HWND window = nullptr;
	WNDCLASSEXA windowClass = {};

	POINTS position = {};

	PDIRECT3D9 d3d = nullptr;
	LPDIRECT3DDEVICE9 dDevice = nullptr;
	D3DPRESENT_PARAMETERS pPresentParams = {};

	void CreateMyWindow(const char* winName, const char* className);
	void DestroyMyWindow(void);
	void ResetDevice(void);
	void DestroyDevice(void);
	bool CreateDevice(void);

	void Install_ImGui(void);
	void Uninstall_ImGui(void);
	void ImGuiBeginRender(void);
	void ImGuiEndRender(void);
	void ImGuiRender();
}



