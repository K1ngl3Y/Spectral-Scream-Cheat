#include "includes.h"
//@Version 1.0.2

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

int stamina_cost = 6, found_coins = 10000, spawn_item_id=10;
//long account_xp = 0;
int account_xp = 0;
float run_speed = 6, move_speed = 2.2f;
bool hp_hack = false, tp_hotkeys = false, action_teleport = true;

PTR GameAssemblyCheat = (PTR)GetModuleHandleA("GameAssembly.dll");

auto domain = reinterpret_cast<uintptr_t(*)()>(reinterpret_cast<LPVOID>(GetProcAddress((HMODULE)GameAssemblyCheat, "il2cpp_domain_get")));
auto thread_attach = reinterpret_cast<uintptr_t(*)(uintptr_t)>(reinterpret_cast<LPVOID>(GetProcAddress((HMODULE)GameAssemblyCheat, "il2cpp_thread_attach")));
auto thread_detach = reinterpret_cast<uintptr_t(*)(uintptr_t)>(reinterpret_cast<LPVOID>(GetProcAddress((HMODULE)GameAssemblyCheat, "il2cpp_thread_detach")));

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

}

enum ItemType
{
	Goods = 10,
	hpPotion = 20,
	Resist = 30,
	Exorcize = 40, // Crash
	ExorcizeGöôoods = 50, // Crash
	Key = 60,
	ChapterKey = 70, 
	Basic = 80, // Crash
	Gimmick = 90,
	ArrowSticker = 100,
	Revive = 110,
	Imp = 120,
	Adrenaline = 130,
	MasterKey = 140,
	PaperButterfly = 150,
	EMF = 160,
	GoldenCompass = 170,
	RedCompass = 180,
	HolyWater = 190,
	Cross = 200,
	Bujeok = 210,
	Bell = 220,
	ExorciseBook = 230,
	Stake = 240,
	Gimmick_Chess = 250,
	Gimmick_Card = 260,
	Gimmick_Battery = 270,
	mhpPotion = 280,
	Collection = 290,
	BoxGoods = 300,
	SSBox = 310,
	NoiseCancel = 320,
	RedButterfly = 330,
	Crossbow = 340,
	Breakable = 350,
	Yarn = 360,
	BloodBowl = 370,
	BossAnnoy = 380,
	JollyChimp = 390,
	Gimmick_Key = 400,
	Rune = 410,
	Hourglass = 420,
	Ragdoll = 430,
	Soulfly = 440
};

enum CheatActionType {
	none,
	InvisibleMode,
	Skin,
	Teleport,
	CompleteChapter,
	GetFoundCoins,
	SetAccountXP,
	SpawnItem,
	EnableConsole
};

void Install(CheatActionType action, int intData = 0) {
	auto HardwareRigClass = *(uintptr_t*)(*(uintptr_t*)(GameAssemblyCheat + 0x511D698) + 0xB8);
	if (!HardwareRigClass) return;

	auto HardwareRig = *(uintptr_t*)(HardwareRigClass + 0x0);
	if (!HardwareRig)return;

	auto NetworkRigLocalPlayer = *(uintptr_t*)(HardwareRig + 0x88);
	if (!NetworkRigLocalPlayer)return;

	auto PlayerState = *(uintptr_t*)(NetworkRigLocalPlayer + 0x120);
	if (!PlayerState)return;

	auto NetworkRig = *(uintptr_t*)(PlayerState + 0xB8);
	if (!NetworkRig)return;

	auto HardwareCharacterController = *(uintptr_t*)(NetworkRig + 0xF0);
	if (!HardwareCharacterController)return;

	*(float*)((uintptr_t)HardwareCharacterController + 0x44) = run_speed; // float runSpeed;
	*(float*)((uintptr_t)HardwareCharacterController + 0x40) = move_speed; // float moveSpeed;
	*(float*)((uintptr_t)HardwareCharacterController + 0x54) = stamina_cost; // float Stamina_Cost

	if (GetAsyncKeyState('H') & 0x8000 && hp_hack)
	{
		auto SetHP = reinterpret_cast<void(*)(uintptr_t, float)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7ACC90));
		if(!SetHP) return;

		*(float*)((uintptr_t)PlayerState + 0x16C) = 10; // Set max health ratio. Default in game 1 == 100 HP. 10 == 1000 HP.

		SetHP(PlayerState, 1000); // 1000 - Health to set.
	}

	if (GetAsyncKeyState('R') & 0x8000) // Hold R to fall down
	{
		auto ResetPosition = reinterpret_cast<void(*)(uintptr_t)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0xA31750));
		if (!ResetPosition) return;
		ResetPosition(HardwareCharacterController);
	}
	if (GetAsyncKeyState('J') & 0x8000) // Allow type cheat commands to chat
	{
		//auto ChatManager = *(uintptr_t*)(*(uintptr_t*)(GameAssemblyCheat + 0x51A3388) + 0xB8);
		//if (!ChatManager) return;

		//auto ChatManagerInstace = *(uintptr_t*)(ChatManager + 0x0);
		//if (!ChatManagerInstace) return;

		//*(bool*)((uintptr_t)ChatManagerInstace + 0xB0) = true;
	}
	if ((GetKeyState(VK_SPACE) & 0x8000) && tp_hotkeys)
	{
		auto GetBodyTargetPosition = reinterpret_cast<Vector3(*)(uintptr_t)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0xA2FF90));
		if (!GetBodyTargetPosition) return;

		auto HardwareRigLocalPlayer = *(uintptr_t*)(NetworkRig + 0xA0);
		if (!HardwareRigLocalPlayer)return;

		auto Teleport = reinterpret_cast<void(*)(uintptr_t, Vector3)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x986260));
		if (!Teleport) return;

		Vector3 bodyPosition = GetBodyTargetPosition(HardwareCharacterController);

		if (GetKeyState(VK_SHIFT) & 0x8000) {
			bodyPosition.y -= 1.5;
			Teleport(HardwareRigLocalPlayer, bodyPosition);
		}
		else if (GetKeyState(VK_RIGHT) & 0x8000) {
			bodyPosition.x -= 1.5;
			Teleport(HardwareRigLocalPlayer, bodyPosition);
		}
		else if (GetKeyState(VK_LEFT) & 0x8000) {
			bodyPosition.x += 1.5;
			Teleport(HardwareRigLocalPlayer, bodyPosition);
		}
		else if (GetKeyState(VK_UP) & 0x8000) {
			bodyPosition.z -= 1.5;
			Teleport(HardwareRigLocalPlayer, bodyPosition);
		}
		else if (GetKeyState(VK_DOWN) & 0x8000) {
			bodyPosition.z += 1.5;
			Teleport(HardwareRigLocalPlayer, bodyPosition);
		}
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		/*auto IamIronman = reinterpret_cast<void(*)()>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7FB140));
		if (!IamIronman) return;

		IamIronman();*/
		/*auto StopCreatures = reinterpret_cast<void(*)()>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7FB460));
		if (!StopCreatures) return;

		StopCreatures();*/
	}
	if (GetKeyState(VK_OEM_3) & 0x8000)
	{
		// Cursor lock
		//auto DesktopCameraController = *(uintptr_t*)(*(uintptr_t*)(GameAssemblyCheat + 0x518B6E8) + 0xB8);
		//if (!DesktopCameraController) return;

		//auto DesktopCameraControllerInstance = *(uintptr_t*)(DesktopCameraController + 0x0);
		//if (!DesktopCameraControllerInstance)return;

		//bool locked = *(bool*)((uintptr_t)DesktopCameraControllerInstance + 0xB4);

		//auto CursorLockToggle = reinterpret_cast<void(*)(uintptr_t)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x96F890));
		//if (!CursorLockToggle) return;

		//CursorLockToggle(DesktopCameraControllerInstance);

		//auto CursorClass = *(uintptr_t*)(*(uintptr_t*)(GameAssemblyCheat + 0x5178E88) + 0xB8);
		//if (!CursorClass) return;

		//auto CursorVisible = reinterpret_cast<void(*)(bool)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x3BCA970));
		//if (!CursorVisible) return;

		//if (!locked) {
		//	CursorVisible(false);
		//}
		//else {
		//	CursorVisible(true);
		//}
	}

	switch (action)
	{
	case InvisibleMode:
	{
		auto Revival = reinterpret_cast<void(*)(uintptr_t, bool)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7AC8A0));
		if (!Revival) return;

		Revival(PlayerState, true);
		break;
	}
	case Skin:
	{
		if (intData == 0) { // Tank
			auto RPC_IamTankRider = reinterpret_cast<void(*)(PTR)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x98CAF0));
			if (!RPC_IamTankRider) return;

			RPC_IamTankRider(NetworkRig);
		}
		else if (intData == 1) { // Saiyan
			auto RPC_IamSaiyan = reinterpret_cast<void(*)(PTR)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x98C8C0));
			if (!RPC_IamSaiyan) return;

			RPC_IamSaiyan(NetworkRig);
		}
		break;
	}
	case Teleport:
	{
		if (intData == 999) {
			auto CheatTeleport_To_Outside = reinterpret_cast<void(*)(PTR, int)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x98DA40));
			if (!CheatTeleport_To_Outside) return;
			CheatTeleport_To_Outside(NetworkRig, 1);
		}
		else {
			auto CheatTeleport_To_Chapter = reinterpret_cast<void(*)(PTR, int)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x98D850));
			if (!CheatTeleport_To_Chapter) return;
			CheatTeleport_To_Chapter(NetworkRig, intData);
		}
		break;
	}

	case CompleteChapter:
	{
		auto CompleteGimmick = reinterpret_cast<void(*)(int)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7FB220));
		if (!CompleteGimmick) return;
		CompleteGimmick(intData);
		break;
	}
	case GetFoundCoins:
	{
		auto RPC_TakePocket = reinterpret_cast<void(*)(PTR, int)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7AA950));
		if (!RPC_TakePocket) return;

		RPC_TakePocket(PlayerState, found_coins);
		break;
	}

	case SetAccountXP:
	{
		auto SetEXP = reinterpret_cast<void(*)(long)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7FAE50));
		if (!SetEXP) return;

		SetEXP(account_xp);
	}
		break;
	case SpawnItem:
	{
		auto Sapwn_Item_Type = reinterpret_cast<void(*)(int)>(reinterpret_cast<LPVOID>(GameAssemblyCheat + 0x7FB6C0)); 
		if (!Sapwn_Item_Type) return;
		if (spawn_item_id >= 10 && spawn_item_id <= 440 && spawn_item_id % 10 == 0) {
			try
			{
				Sapwn_Item_Type(spawn_item_id);
			}
			catch (...)
			{

			}

		}
		
		break;
	}
	case EnableConsole:
	{
		auto ChatManager = *(uintptr_t*)(*(uintptr_t*)(GameAssemblyCheat + 0x51715B0) + 0xB8);
		if (!ChatManager) return;

		auto ChatManagerInstace = *(uintptr_t*)(ChatManager + 0x0);
		if (!ChatManagerInstace) return;

		*(bool*)((uintptr_t)ChatManagerInstace + 0xB0) = true;
	}

	default:
		break;
	}

}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	uintptr_t thread = thread_attach(domain());
	Install(none);
	//ImGui::SetNextWindowSize({ 380.f,262.f });

	ImGui::Begin("Spectral Scream | K1ngl3Y");
	ImGui::SetCursorPos({ 10.f,30.f });
	if (ImGui::Button("Invisible Mode", { 117.f,29.f }))
	{
		Install(InvisibleMode);
	}
	ImGui::SetCursorPos({ 131.f,30.f });
	if (ImGui::Button("Skin Tanker", { 117.f,29.f }))
	{
		Install(Skin, 0);
	}
	ImGui::SetCursorPos({ 252.f,30.f });
	if (ImGui::Button("Skin Sayan", { 117.f,29.f }))
	{
		Install(Skin, 1);
	}
	ImGui::SetCursorPos({ 10.f,90.f });
	if (ImGui::Button("Chapter 1", { 71.f,19.f }))
	{
		action_teleport ? Install(Teleport, 1) : Install(CompleteChapter, 1);
	}
	ImGui::SetCursorPos({ 85.f,90.f });
	if (ImGui::Button("Chapter 2", { 71.f,19.f }))
	{
		action_teleport ? Install(Teleport, 2) : Install(CompleteChapter, 2);
	}
	ImGui::SetCursorPos({ 160.f,90.f });
	if (ImGui::Button("Chapter 3", { 71.f,19.f }))
	{
		action_teleport ? Install(Teleport, 3) : Install(CompleteChapter, 3);
	}
	ImGui::SetCursorPos({ 235.f,90.f });
	if (ImGui::Button("Chapter 4", { 71.f,19.f }))
	{
		action_teleport ? Install(Teleport, 4) : Install(CompleteChapter, 4);
	}
	ImGui::SetCursorPos({ 310.f,90.f });
	if (ImGui::Button("Lobby", { 60.f,19.f }))
	{
		Install(Teleport, 999);
	}
	ImGui::SetCursorPos({ 14.f,68.f });
	ImGui::PushItemWidth(105.000000);
	ImGui::Text("Set action type");
	ImGui::PopItemWidth();
	ImGui::SetCursorPos({ 10.f,123.f });
	if (ImGui::Button("Get found Coins", { 114.f,27.f }))
	{
		Install(GetFoundCoins);
	}
	ImGui::SetCursorPos({ 130.f,127.f });
	ImGui::PushItemWidth(155.000000);
	ImGui::InputInt("Coins count", &found_coins, 100, 1000);
	ImGui::PopItemWidth();

	ImGui::SetCursorPos({ 11.f,202.f });
	ImGui::PushItemWidth(260.000000);
	ImGui::SliderInt("Stamina Cost", &stamina_cost, 0, 100);
	ImGui::PopItemWidth();

	ImGui::SetCursorPos({ 10.f,225.f });
	ImGui::PushItemWidth(110.000000);
	ImGui::SliderFloat("Run Speed", &run_speed, 0, 30, "%.1f");
	ImGui::PopItemWidth();

	ImGui::SetCursorPos({ 190.f,225.f });
	ImGui::PushItemWidth(105.000000);
	ImGui::SliderFloat("Move Speed", &move_speed, 0, 30, "%.1f");
	ImGui::PopItemWidth();

	ImGui::SetCursorPos({ 10.f,165.f });
	ImGui::Checkbox("Enable Health Hack", &hp_hack);

	ImGui::SetCursorPos({ 185.f,165.f });
	ImGui::Checkbox("Enable Teleport HotKeys", &tp_hotkeys);

	ImGui::SetCursorPos({ 11.f,256.f });
	if (ImGui::Button("Set account xp", { 107.f,19.f }))
	{
		Install(SetAccountXP);
	}
	ImGui::SetCursorPos({ 124.f,256.f });
	ImGui::PushItemWidth(230.000000);
	ImGui::InputInt("XP", &account_xp, 100, 1000);
	ImGui::PopItemWidth();

	ImGui::SetCursorPos({ 138.f,66.f });
	if (ImGui::Button("Teleport to", { 110.f,19.f }))
	{
		action_teleport = true;
	}
	ImGui::SetCursorPos({ 251.f,66.f });
	if (ImGui::Button("Complete chapter", { 118.f,19.f }))
	{
		action_teleport = false;
	}
	ImGui::SetCursorPos({ 10.f,281.f });
	if (ImGui::Button("Spawn item by type ID", { 155.f,19.f }))
	{
		Install(SpawnItem);
	}
	ImGui::SetCursorPos({ 174.f,280.f });
	ImGui::PushItemWidth(180.000000);
	ImGui::InputInt("ID", &spawn_item_id, 10, 10);
	ImGui::PopItemWidth();


	//ImGui::SetCursorPos({ 10.f,305.f });
	//if (ImGui::Button("Enable chat commands", { 155.f,19.f }))
	//{
	//	Install(EnableConsole);
	//}

	ImGui::End();

	thread_detach(thread);
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);


		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}