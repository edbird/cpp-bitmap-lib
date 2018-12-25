#ifndef PIXELRGB_HPP
#define PIXELRGB_HPP


// C++ headers
#include <cstdint>


namespace BMP
{


    //union PixelRGB
    class PixelRGB
    {

    public:

        uint8_t array[3];
        /*
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
        */
        uint8_t &r;
        uint8_t &g;
        uint8_t &b;


    public:

        PixelRGB()
            : r{array[2]}
            , g{array[1]}
            , b{array[0]}
        {
        }

        PixelRGB& operator=(const PixelRGB& pixel)
        {
            for(int c{0}; c < 3; ++ c) array[c] = pixel.array[c];
            return *this;
        }


        PixelRGB& operator&=(const PixelRGB& pixel)
        {
            b &= pixel.b;
            g &= pixel.g;
            r &= pixel.r;
            return *this;
        }
        PixelRGB& operator|=(const PixelRGB& pixel)
        {
            b |= pixel.b;
            g |= pixel.g;
            r |= pixel.r;
            return *this;
        }
        PixelRGB& operator^=(const PixelRGB& pixel)
        {
            b ^= pixel.b;
            g ^= pixel.g;
            r ^= pixel.r;
            return *this;
        }

    };

    PixelRGB operator&(const PixelRGB&, const PixelRGB&);
    PixelRGB operator|(const PixelRGB&, const PixelRGB&);
    PixelRGB operator^(const PixelRGB&, const PixelRGB&);

}

#endif // PIXELRGB_HPP
