

#include "cr.h"
#include <cassert>
#include <iostream>
#include <windows.h>

class StaticModule
{
public:
    virtual void Initialize() {}
    virtual void Tick() {}
    virtual void Terminate() {}
};

CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation) {
    assert(ctx);
    switch (operation) {
        case CR_LOAD:
        {
            std::cout << "Load1\n";

            // loading back from a reload
            return 0;
        }
        case CR_UNLOAD:
        {
            std::cout << "Unload\n";

            // preparing to a new reload
            return 0;
        }
        case CR_CLOSE:
        {
            std::cout << "Close\n";

            // the plugin will close and not reload anymore
            return 0;
        }
        case CR_STEP:
        {
            //std::cout << "Update\n";
            return 0;
        }
    }

    assert(false);
    return 0;
}

#pragma region DaFuDoesThisDo
// Do i really need this?
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

int WINAPI WinMain(      
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{  
  return 0;
}
#pragma endregion