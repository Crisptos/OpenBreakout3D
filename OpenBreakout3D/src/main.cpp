#include "vk_render_engine.h"

int main(int argc, char **argv)
{
    OB3D::RenderEngine engine;

    engine.Init();
    engine.Run();
    engine.Destroy();
    return 0;
}