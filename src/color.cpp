#include "color.h"

Color::Color(int r, int g, int b)
{
    m_rgb = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

Color Color::random()
{   
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,255);

    return Color(dist6(rng), dist6(rng), dist6(rng));
}