#include "main.h"

ImFont* pFontAwesomeSolid = nullptr, *pFontAwesomeReg = nullptr, * pFontAwesomeBrands = nullptr, * pFontArial = nullptr;

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
		case WM_SIZE: 
		{
			
			if (Render::dDevice && wideParameter != SIZE_MINIMIZED)
			{
				Render::pPresentParams.BackBufferWidth = LOWORD(longParameter);
				Render::pPresentParams.BackBufferHeight = HIWORD(wideParameter);
				Render::ResetDevice();
			}
		}return 0;

		case WM_SYSCOMMAND: {
			if ((wideParameter & 0xfff0) == SC_KEYMENU)
				return 0;
		}break;

		case WM_DESTROY: {
			PostQuitMessage(0);
		}return 0;

		case WM_LBUTTONDOWN: {
			Render::position = MAKEPOINTS(longParameter);
		}return 0;


		case WM_MOUSEMOVE:
		{
			if (wideParameter == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(longParameter);
				auto rect = RECT{};
				GetWindowRect(Render::window, &rect);
				rect.left += points.x - Render::position.x;
				rect.top += points.y - Render::position.y;

				if (Render::position.x >= 0 &&
					Render::position.x <= Render::pPresentParams.BackBufferWidth &&
					Render::position.y >= 0 &&
					Render::position.y <= Render::pPresentParams.BackBufferHeight) //if you want to move only by the top put 30 or less
				{
					SetWindowPos(Render::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}return 0;

		default:
			return DefWindowProcA(window, message, wideParameter, longParameter);

	}
	return 0;
}

void Render::CreateMyWindow(const char* winName, const char* className)
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, winName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void Render::DestroyMyWindow(void)
{
	DestroyWindow(Render::window);
	::UnregisterClassA(Render::windowClass.lpszClassName, Render::windowClass.hInstance);
}

bool Render::CreateDevice(void)
{

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!Render::d3d)
		return false;

	ZeroMemory(&pPresentParams, sizeof(pPresentParams));

	pPresentParams.Windowed = TRUE;
	pPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	pPresentParams.EnableAutoDepthStencil = TRUE;
	pPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	pPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pPresentParams, &dDevice) < 0)
		return false;

	return true;
}

void Render::ResetDevice(void)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = Render::dDevice->Reset(&pPresentParams);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void Render::DestroyDevice(void)
{
	if (Render::dDevice)
	{
		Render::dDevice->Release();
		Render::dDevice = nullptr;
	}

	if (Render::d3d)
	{
		Render::d3d->Release();
		Render::d3d = nullptr;
	}
}

void Render::Install_ImGui(void)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	//main font
	pFontArial = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	IM_ASSERT(pFontArial != NULL);

	static const ImWchar icons_rangesA[] = { ICON_MIN_FA, ICON_MAX_FA, 0 }; 
	static const ImWchar icons_rangesB[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 }; 

	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	pFontAwesomeReg = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAR, 16.0f, &icons_config, icons_rangesA);
	IM_ASSERT(pFontAwesomeReg != NULL);

	pFontAwesomeSolid = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_rangesA);
	IM_ASSERT(pFontAwesomeSolid != NULL);

	pFontAwesomeBrands = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAB, 16.0f, &icons_config, icons_rangesB);
	IM_ASSERT(pFontAwesomeBrands != NULL);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(Render::dDevice);

}
void Render::Uninstall_ImGui(void)
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Render::ImGuiBeginRender(void)
{
	MSG msg;

	while (PeekMessage(&msg, 0,0,0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Render::ImGuiEndRender(void)
{
	ImGui::EndFrame();

	Render::dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	Render::dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Render::dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	Render::dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (Render::dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		Render::dDevice->EndScene();
	}

	//Handle loss of the D3D9 device
	const auto resl = Render::dDevice->Present(0, 0, 0, 0);
	if (resl == D3DERR_DEVICELOST && Render::dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		Render::ResetDevice();

}

void Render::ImGuiRender()
{
	Utilite* Utils;
	static char szFile[260] = "";
	static char szFilter[260] = "DLL/ASI\0*.DLL; *.ASI\0 All\0*.*\0";
	static ImGuiWindowFlags winFlags = {
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse };
	ImVec2 mainWindowPos;
	static int tab_selected = 0;
	ImGui::SetNextWindowPos({ (float)0, (float)0 });
	ImGui::SetNextWindowSize({ (float)WIDTH, (float)HEIGHT });
	if (ImGui::Begin("Ultimate Injector v1.0", &active, winFlags))
	{
		ImGui::PushFont(pFontAwesomeReg);

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "ULTIMATE INJECTOR");
		ImGui::SameLine(363);
		if (ImGui::Button("X", ImVec2(30, 29))) {
			ExitProcess(0);
		}
		ImGui::Separator();
		if (ImGui::Button("Injector", ImVec2(130, 30))) {
			tab_selected = 0;
			ImGui::SameLine();
		}
		ImGui::SameLine();
		if (ImGui::Button("About", ImVec2(130, 30))) {
			tab_selected = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Contacts", ImVec2(130, 30))) {
			tab_selected = 2;
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (tab_selected == 0)
		{
			std::vector<std::pair<std::string, DWORD>> process_list = Utils->GetProcessInfo();
			const char** process_info_array = new const char* [process_list.size()];
			for (size_t i = 0; i < process_list.size(); i++)
			{
				process_info_array[i] = process_list[i].first.c_str();
			}
			static int selected_process = -1;
			if (ImGui::Combo(" ", &selected_process, process_info_array, (int)process_list.size()))
			{
			}
			DWORD process_id = process_list[selected_process].second;
			if (process_id > 100000){}
			else
			{
				std::stringstream ss;
				ss << "Process ID: " << process_id;
				ImGui::SameLine(270);
				ImGui::Text(ss.str().c_str());
			}
			Utils->ShowFileOpenDialog(szFile, sizeof(szFile), szFilter);
			ImGui::SameLine(100);
			std::string path = szFile;
			std::string file = path.substr(path.rfind('\\') + 1);
			if (szFile != "")
				ImGui::Text(file.c_str());
			for (int i = 0; i < 5; i++) {
				ImGui::Spacing();
			}
			ImGui::SameLine(100);
			if (ImGui::Button("INJECT!", ImVec2(200, 80)))
			{
				if (Utils->LoadLibraryAMethod(process_id, szFile))
				{
					ImGui::OpenPopup("Injection Successful");
				}
				else
				{
					ImGui::OpenPopup("Injection Failed");
				}
			}
		}
		if (ImGui::BeginPopupModal("Injection Successful", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The DLL was successfully injected into the target process.");
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Injection Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The injection failed. Make sure the process ID and DLL path are correct.");
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Injection Failed - Invalid PID", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The injection failed. Make sure the process ID is selected!");
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Injection Failed - Invalid Path", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The injection failed. Make sure the DLL path is correct!");
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		else if (tab_selected == 1)
		{
			ImGui::Text("Injector with LLA method.");
			ImGui::Text("Created for x86[32-bit] processes.");
			ImGui::Text("You can see the source at: github.com/takiyamaqw");
		}
		else if (tab_selected == 2)
		{
			ImGui::Text("vk.com/takiyama");
			ImGui::Text("github.com/takiyamaqw");
		}
		ImGui::PopFont();
		ImGui::End();
	}
}


//create thread for render
int __stdcall wWinMain(HINSTANCE instance, HINSTANCE lastInstance, PWSTR args, int cmdShow)
{
	Render::CreateMyWindow("Ultimate Injector v1.0", "Injector");
	Render::CreateDevice();
	Render::Install_ImGui();

	while (Render::active)
	{
		Render::ImGuiBeginRender();
		Render::ImGuiRender();
		Render::ImGuiEndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Render::Uninstall_ImGui();
	Render::DestroyDevice();
	Render::DestroyMyWindow();

	return EXIT_SUCCESS;
}
