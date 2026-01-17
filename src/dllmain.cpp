#define WIN32_LEAN_AND_MEAN
#define modName "eldenringNGPlus"

#include <windows.h>
#include <scanlib.h>
#include <tinyfiledialogs.h>
#include <iostream>
#include <string>

#include "utils.h"
#include "structures.h"

#define loop for (;;)
constexpr DWORD fps50ms = 1000 / 50;

GameDataMan **gameDataManAddr;

void modMain()
{
    std::cout << "Starting " << modName << std::endl;

    MemoryPattern gameDataManScan{"48 8B 05 ? ? ? ? 48 85 C0 74 05 48 8B 40 58 C3 C3"};
    Scanlib_AddPattern(&gameDataManScan);

    ScanlibResult res = Scanlib_Search();
    if (res != SCANLIB_OK)
    {
        std::cerr << "Error scanning memory " << modName << std::endl;
        return;
    }

    if (gameDataManScan.foundAddr)
        std::cout << "gameDataMan found at: " << gameDataManScan.foundAddr << std::endl;
    else
    {
        std::cerr << "gameDataMan not found " << modName << std::endl;
        return;
    }

    { // gameDataMan
        uintptr_t offset = *reinterpret_cast<uint32_t *>(gameDataManScan.foundAddr + 3);
        uintptr_t baseAddr = gameDataManScan.foundAddr + 7;
        gameDataManAddr = reinterpret_cast<GameDataMan **>(baseAddr + offset);
    }

    std::cout << "Correctly loaded " << modName << std::endl;

    loop
    {
        if (gameDataManAddr &&
            *gameDataManAddr &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
            (GetAsyncKeyState('P') & 0x8000))
        {
            const char *input = tinyfd_inputBox(
                "Change new game plus level",
                "Enter new NG+ level",
                std::to_string((*gameDataManAddr)->ngLvl).c_str());

            if (input)
            {
                unsigned int newNGval;
                bool parsedVal = strToUInt(input, newNGval);
                if (parsedVal)
                {
                    std::cout << "Value entered: " << newNGval << std::endl;
                    (*gameDataManAddr)->ngLvl = newNGval;
                }
                else
                {
                    std::cout << "Invalid input" << std::endl;
                }
            }
            else
            {
                std::cout << "Input canceled" << std::endl;
            }
        }

        // run this thing at around 50 fps
        Sleep(fps50ms);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD reasonForCall,
                      LPVOID lpReserved)
{
    switch (reasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&modMain, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
