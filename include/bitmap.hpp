#ifndef BITMAP_HPP
#define BITMAP_HPP


// Local headers
#include "pixelrgb.hpp"

// C++ headers
#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>


namespace BMP
{


    typedef uint8_t BYTE;
    typedef uint16_t WORD;
    typedef uint32_t DWORD;
    typedef uint64_t LONG;





    enum class KernelMode
    {
        UNDEFINED,
        AND,
        OR,
        XOR
    };


    class FunctorKernel
    {

        KernelMode mode;

    public:
    
        FunctorKernel()
            : mode(KernelMode::UNDEFINED)
        {
        }

        FunctorKernel(const KernelMode mode)
            : mode(mode)
        {
        }

        // binary kernel
        void operator()(uint8_t * const output, const uint8_t * const l, const uint8_t * const r, const int count = 1)
        {
            for(int c{0}; c < count; ++ c)
            {
                if(mode == KernelMode::UNDEFINED)
                {
                    // nothing
                }
                else if(mode == KernelMode::AND)
                {
                    output[c] = l[c] & r[c];
                }
                else if(mode == KernelMode::OR)
                {
                    output[c] = l[c] | r[c];
                }
                else if(mode == KernelMode::XOR)
                {
                    output[c] = l[c] ^ r[c];
                }
            }
        }

        // unary kernel
        void operator()(uint8_t * const output, const uint8_t * const input, const int count = 1)
        {
            for(int c{0}; c < count; ++ c)
            {
                if(mode == KernelMode::UNDEFINED)
                {
                    // nothing
                }
                else if(mode == KernelMode::AND)
                {
                    output[c] &= input[c];
                }
                else if(mode == KernelMode::OR)
                {
                    output[c] |= input[c];
                }
                else if(mode == KernelMode::XOR)
                {
                    output[c] ^= input[c];
                }
            }
        }


        // binary kernel
        void operator()(PixelRGB& output, const PixelRGB& input_l, const PixelRGB& input_r)
        {
            if(mode == KernelMode::UNDEFINED)
            {
                // nothing
            }
            else if(mode == KernelMode::AND)
            {
                output = input_l & input_r;
            }
            else if(mode == KernelMode::OR)
            {
                output = input_l | input_r;
            }
            else if(mode == KernelMode::XOR)
            {
                output = input_l ^ input_r;
            }
        }

        // unary kernel
        void operator()(PixelRGB& output, const PixelRGB& input)
        {
            if(mode == KernelMode::UNDEFINED)
            {
                // nothing
            }
            else if(mode == KernelMode::AND)
            {
                output &= input;
            }
            else if(mode == KernelMode::OR)
            {
                output |= input;
            }
            else if(mode == KernelMode::XOR)
            {
                output ^= input;
            }
        }
        

    };




    // bitmap surface class
    // base class for bitmap canvas and bitmap font classes
    // contains method for blit (copy)
    // no drawing, no saving / loading from file in any format
    // TODO: maybe the load / save as bitmap, png etc should go in this function?
    class BITMAP
    {


    protected:

        LONG m_width; // width of bitmap (pixels)
        LONG m_height; // height of bitmap (pixels)
        WORD m_bit_count; // bits per pixel
        LONG m_width_pad; // = (4 - (3 * m_size_x) % 4) % 4; (bytes)
        LONG m_width_memory; // not same as width, includes padding (bytes)
        std::vector<uint8_t> m_data; // bitmap data


        struct BITMAPFILEHEADER
        {
            WORD  bfType; // 2
            DWORD bfSize; // 6
            WORD  bfReserved1; // 8
            WORD  bfReserved2; // 10
            DWORD bfOffBits; // 14
        }__attribute__((packed)); //m_f_head;


        struct BITMAPINFOHEADER //tagBITMAPINFOHEADER
        {
            DWORD biSize; // 4
            DWORD biWidth; // 8 //LONG  biWidth; // 12
            DWORD biHeight; // 12 //LONG  biHeight; // 20
            WORD  biPlanes; // 22 - 8
            WORD  biBitCount; // 24 - 8
            DWORD biCompression; // 28 - 8
            DWORD biSizeImage; // 32 - 8
            DWORD biXPelsPerMeter; //LONG  biXPelsPerMeter; // 40 - 8 - 4
            DWORD biYPelsPerMeter; //LONG  biYPelsPerMeter; // 48 - 8 - 8 = 40 - 8
            DWORD biClrUsed; // 52 - 8 - 8
            DWORD biClrImportant; // 56 - 8 - 8 = 40
        }__attribute__((packed)); //m_i_head; // 56 + 14 = 70


        // convert x,y coordinate to array index (pixel index in memory)
        //inline
        //uint64_t index(const uint32_t x, const uint32_t y)
        //{
        //    uint64_t index = ((uint64_t)3) * (((uint64_t)x) + ((uint64_t)y) * ((uint64_t)m_width_memory));
        //    return index;
        //}
        inline
        uint64_t index(const LONG x, const LONG y) const;

        // Convert (assumed 32 bit) integer into an array of 4 characters, in reverse order
        // This is required for setting the file length in the BITMAP header, etc
        //void int_to_char_array_reversed(unsigned int input, unsigned char* output) const
        //{
        //  output[3] = (input & 0xFF000000) >> 0x18;
        //  output[2] = (input & 0x00FF0000) >> 0x10;
        //  output[1] = (input & 0x0000FF00) >> 0x08;
        //  output[0] = (input & 0x000000FF) >> 0x00;
        //
        //  return;
        //}


        // Endian-reverse conversion functions for 16bit, 32bit, 64bit words
        uint16_t ushort_rev(const uint16_t data) const;
        uint32_t uint_rev(const uint32_t data) const;
        uint64_t ulong_rev(const uint64_t data) const;

        // TODO: another verion with returns bool true/false depending on if period is found
        std::string filename_extension(const std::string& str, std::string* str_no_ext_p) const;
            // str_no_ext_p is a pointer to a string which holds the contents of str with the file extension
            // and period removed from the end (no extension)
            // if this is nullptr, then the argument is not set

        // TODO: these should be general functions not specific to this class
        inline
        std::string filename_extension(const std::string& str) const;


    private:

        // swap function for exception safety / efficiency
        friend
        void swap(BMP::BITMAP& l, BMP::BITMAP& r);

        // TODO: WARNING: bit_count ONLY WORKS FOR 24, 32 BIT IMAGES! (due to / 8 operation)


    protected:

        void
        reinitialize(const LONG width, const LONG height, const WORD bit_count);


    public:

        //explicit
	    //BITMAPCanvas(const uint32_t x, const uint32_t y, const BITMAPFont* const bmpfont_ptr)
	    //	: m_data(3 * x * y) // enough data for x * y pixels, RGB format
	    //	, m_size_x{x}
	    //	, m_size_y{y}
	    //{
        //    // clear canvas
        //    DrawRect(0, 0, m_size_x - 1, m_size_y - 1, Color(255));
        //}


        // blank constructor required for loading from file (before properties are known)
        BITMAP();

        // regular constructor
        // blank constructor (create new file in memory)
        BITMAP(const LONG width, const LONG height, const WORD bit_count);

        // loader constructor (load from file)
        BITMAP(const std::string& filename);

        virtual
	    ~BITMAP();

        // Copy constructor
	    BITMAP(const BITMAP& bmpsurface);

        // Move constructor
	    BITMAP(BITMAP&& bmpsurface);

        // Copy and move assignment operator using swap() function
        // See MANUAL
	    BITMAP& operator=(BITMAP bmpsurface);

        virtual
        void Load(const std::string& filename);

        // Automatic save as, detects file format depending on
        // extension to filename supplied.
        // eg, bmp format class,
        // png format class
        // and one which inherits from all which can save as all raster graphics formats
        virtual // future classes should over-ride this depending on what formats that class can save
        void SaveAs(const std::string& filename) const;

        virtual
        std::vector<unsigned char> SaveMem() const;

        void LoadBITMAP(const std::string& filename);

        // Saves data in array to file with correctly formatted
        // bitmap file header.
        void SaveAsBitmap(const std::string& filename) const;

        void Clear();

        //std::vector<uint8_t>& Data();


        ////////////////////////////////////////////////////////////////////////
        // resize
        ////////////////////////////////////////////////////////////////////////

        // dumb algorithm
        void Resize(const int width, const int height);

        ////////////////////////////////////////////////////////////////////////
        // translation
        ////////////////////////////////////////////////////////////////////////

        void Translate(const int dx, const int dy);

        ////////////////////////////////////////////////////////////////////////
        // filters
        ////////////////////////////////////////////////////////////////////////

        
        // abstraction to unary and binary pixel operations
        // bounds checked binary/unary operator iterators

        // apply a unary kernel to *this with argument of another bmp image
        void OperatorKernelUnary(const BITMAP& bitmap, FunctorKernel kernel);

        // apply a binary kernel to *this with argument of 2 other bmp images
        void OperatorKernelBinary(const BITMAP& bitmap_l, const BITMAP& bitmap_r, FunctorKernel kernel);
        // TODO: same but with arguments where l or r is uint8_t[3]

        // apply a unary kernel to *this with argument of rgb as pointer to pixels
        void RGBOperatorKernelUnary(const uint8_t* const rgb, FunctorKernel kernel);

        // apply a unary kernel to *this with argument of rgb as array of uin8_t[3]
        //void RGBOperatorKernelUnary(const uint8_t rgb[3], FunctorKernel kernel);

        // apply a unary kernel to *this with argument of rgb as separated uint8_t color component values
        void RGBOperatorKernelUnary(const uint8_t r, const uint8_t g, const uint8_t b, FunctorKernel kernel);
        //void RGBOperatorKernelBinary(const uint8_t* const rgb, FunctorKernel kernel); // does not make sense?



        // assume 24bit bitmap format
        void RGBFilterGeneric(const uint8_t r, const uint8_t g, const uint8_t b, FunctorKernel functorkernel);
        void RGBFilterAND(const uint8_t r, const uint8_t g, const uint8_t b);
        void RGBFilterOR(const uint8_t r, const uint8_t g, const uint8_t b);
        void RGBFilterXOR(const uint8_t r, const uint8_t g, const uint8_t b);

        void Generic(const BITMAP& bitmap, FunctorKernel functorkernel);
        void AND(const BITMAP& bitmap);
        void OR(const BITMAP& bitmap);
        void XOR(const BITMAP& bitmap);

    };




}

#endif
