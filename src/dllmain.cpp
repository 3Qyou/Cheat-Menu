#include "pch.h"
#include "hook.h"
#include "cheatmenu.h"
#include "updater.h"
#include "menuinfo.h"

void MenuThread(void* param)
{
	static bool bGameInit = false;

	// Wait till game init
	Events::initRwEvent += []
	{
		bGameInit = true;
	};

	while (!bGameInit)
	{
		Sleep(1000);
	}

	/*
		Had to put this in place since some people put the folder in root 
		directory and the asi in modloader. Why??
	*/
	if (!std::filesystem::is_directory(PLUGIN_PATH((char*)"CheatMenu")))
	{
		gLog << "Error: CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory" << std::endl;
		MessageBox(RsGlobal.ps->window, "CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory", "CheatMenu", MB_ICONERROR);
		return;
	}

	/*
		Need SilentPatch since all gta games have issues with mouse input
		Implementing mouse fix is a headache anyway
	*/
	if (!GetModuleHandle(BY_GAME("SilentPatchSA.asi" ,"SilentPatchVC.asi" ,"SilentPatchIII.asi")))
	{
		gLog << "Error: SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/" << std::endl;
		int msgID = MessageBox(RsGlobal.ps->window, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", "CheatMenu", MB_OKCANCEL | MB_DEFBUTTON1);

		if (msgID == IDOK)
		{
			ShellExecute(nullptr, "open", "https://gtaforums.com/topic/669045-silentpatch/", nullptr, nullptr, SW_SHOWNORMAL);
		};
		return;
	}

#ifdef GTASA
    /*
        TODO: Find a better way
        Since you could still name it something else
    */
	if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
	{
		gLog << "Error: CheatMenu doesn't support SAMP" << std::endl;
		MessageBox(RsGlobal.ps->window, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
		return;
	}
	CFastman92limitAdjuster::Init();
#endif

	gLog << "Starting...\nVersion: " MENU_TITLE "\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nMore Info: "
		GITHUB_LINK "\n" << std::endl;
	CheatMenu menu;


	// Checking for updates once a day
	time_t now = time(0);
	struct tm  tstruct = *localtime(&now);
	int lastCheckDate = gConfig.GetValue("config.last_update_checked", 0);

	if (lastCheckDate != tstruct.tm_mday)
	{
		Updater::CheckForUpdate();
		gConfig.SetValue("config.last_update_checked", tstruct.tm_mday);
	}

	while (true)
	{
		Sleep(5000);

		if (Updater::m_State == UPDATER_CHECKING)
		{
			Updater::CheckForUpdate();
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		uint gameVersion = GetGameVersion();
		
		if (gameVersion == BY_GAME(GAME_10US_HOODLUM, GAME_10EN, GAME_10EN))
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
		}
		else
		{
			gLog << "Error: Unknown game version. GTA " <<  BY_GAME("SA v1.0 US Hoodlum", "GTA VC v1.0 EN", "GTA III v1.0 EN") << " is required." << std::endl;
#ifdef GTASA
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA SA v1.0 US Hoodlum is required.", "CheatMenu", MB_ICONERROR);
#elif GTAVC
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA VC v1.0 EN is required.", "CheatMenu", MB_ICONERROR);
#else // GTA3
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA III v1.0 EN is required.", "CheatMenu", MB_ICONERROR);
#endif
		}
	}

	return TRUE;
}