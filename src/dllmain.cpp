#define WIN32_LEAN_AND_MEAN
#define modName "eldenringNGPlus"

#include <windows.h>
#include <scanlib.h>
#include <tinyfiledialogs.h>
#include <iostream>
#include <string>

#include "structures.h"

#define loop for (;;)
constexpr DWORD fps50ms = 1000 / 50;

GameDataMan **gameDataManAddr;

bool strToU32(const char *input, unsigned int &outValue)
{
    if (!input || *input == '\0')
        return false;

    errno = 0;
    char *endPtr = nullptr;
    unsigned long val = std::strtoul(input, &endPtr, 10);

    if (errno == ERANGE || val > UINT_MAX)
        return false;

    if (*endPtr != '\0')
        return false;

    outValue = static_cast<unsigned int>(val);
    return true;
}

void modMain()
{
    std::cout << "Starting " << modName << std::endl;
    SetProcessDPIAware();

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
            *gameDataManAddr)
        {
            if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
                (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
                (GetAsyncKeyState('P') & 0x8000))
            {
                const char *input = tinyfd_inputBox(
                    "Change new game plus level",
                    "Enter new NG+ level",
                    std::to_string((*gameDataManAddr)->ng_lv).c_str());

                if (input)
                {
                    unsigned int newNGval;
                    bool parsedVal = strToU32(input, newNGval);
                    if (parsedVal)
                    {
                        std::cout << "Value entered: " << newNGval << std::endl;
                        (*gameDataManAddr)->ng_lv = newNGval;
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
