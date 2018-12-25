#include "pixelrgb.hpp"


BMP::PixelRGB BMP::operator&(const BMP::PixelRGB& pixel_l, const BMP::PixelRGB& pixel_r)
{
    BMP::PixelRGB ret;
    ret.r = pixel_l.r & pixel_r.r;
    ret.g = pixel_l.g & pixel_r.g;
    ret.b = pixel_l.b & pixel_r.b;
    return ret;
}

BMP::PixelRGB BMP::operator|(const BMP::PixelRGB& pixel_l, const BMP::PixelRGB& pixel_r)
{
    BMP::PixelRGB ret;
    ret.r = pixel_l.r | pixel_r.r;
    ret.g = pixel_l.g | pixel_r.g;
    ret.b = pixel_l.b | pixel_r.b;
    return ret;
}

BMP::PixelRGB BMP::operator^(const BMP::PixelRGB& pixel_l, const BMP::PixelRGB& pixel_r)
{
    BMP::PixelRGB ret;
    ret.r = pixel_l.r ^ pixel_r.r;
    ret.g = pixel_l.g ^ pixel_r.g;
    ret.b = pixel_l.b ^ pixel_r.b;
    return ret;
}
