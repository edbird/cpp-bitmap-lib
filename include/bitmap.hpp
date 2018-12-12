#ifndef BITMAP_HPP
#define BITMAP_HPP


#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>

namespace BITMAP
{


    typedef uint8_t BYTE;
    typedef uint16_t WORD;
    typedef uint32_t DWORD;
    typedef uint64_t LONG;


    // bitmap surface class
    // base class for bitmap canvas and bitmap font classes
    // contains method for blit (copy)
    // no drawing, no saving / loading from file in any format
    // TODO: maybe the load / save as bitmap, png etc should go in this function?
    class BMP
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
        inline
        //uint64_t index(const uint32_t x, const uint32_t y)
        //{
        //    uint64_t index = ((uint64_t)3) * (((uint64_t)x) + ((uint64_t)y) * ((uint64_t)m_width_memory));
        //    return index;
        //}
        LONG index(const LONG x, const LONG y) const
        {
            // note m_width_memory has unit of BYTE
            // x has units of (index)
            // y has units of (index)
            // m_bit_count has units of BYTE
            uint64_t index = ((m_bit_count / 8) * x + y * m_width_memory);
            return index;
        }


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
        uint16_t ushort_rev(const uint16_t data) const
        {
            return ( ((data & 0xFF00) >> 0x08) |
                     ((data & 0x00FF) << 0x08) );
        }


        uint32_t uint_rev(const uint32_t data) const
        {
            return (
                    ((data & 0xFF000000) >> 0x18) |
                    ((data & 0x00FF0000) >> 0x08) |
                    ((data & 0x0000FF00) << 0x08) |
                    ((data & 0x000000FF) << 0x18) );
        }

        
        uint64_t ulong_rev(const uint64_t data) const
        {
            return (
                    ((data & 0xFF00000000000000) >> 0x38) |
                    ((data & 0x00FF000000000000) >> 0x28) |
                    ((data & 0x0000FF0000000000) >> 0x18) |
                    ((data & 0x000000FF00000000) >> 0x08) |
                    ((data & 0x00000000FF000000) << 0x08) |
                    ((data & 0x0000000000FF0000) << 0x18) |
                    ((data & 0x000000000000FF00) << 0x28) |
                    ((data & 0x00000000000000FF) << 0x38) );
        }


        // TODO: another verion with returns bool true/false depending on if period is found
        std::string filename_extension(const std::string& str, std::unique_ptr<std::string> str_no_ext_p) const
            // str_no_ext_p is a pointer to a string which holds the contents of str with the file extension
            // and period removed from the end (no extension)
            // if this is nullptr, then the argument is not set
        {
            std::string::size_type index{str.rfind('.')};

            if(index != std::string::npos)
            {
                // TODO: test this
                //std::cout << "filename is " << str.substr(0, str.size() - index) << " extension is " << str.substr(index + 1) << std::endl;
                if(str_no_ext_p.get() != nullptr)
                {
                    str_no_ext_p.get()->operator=(str.substr(0, str.size() - index)); // extract string without extension
                }
                return str.substr(index + 1);
            }
            else
            {
                //std::cerr << "no char '.' found" << std::endl; // this is just a test for now
                if(str_no_ext_p.get() != nullptr)
                {
                    str_no_ext_p.get()->operator=(str); // set string without extension to be full string
                }
                return std::string(""); // return empty string, no period found
            }

        }


        // TODO: these should be general functions not specific to this class
        inline
        std::string filename_extension(const std::string& str) const
        {
            return filename_extension(str, std::unique_ptr<std::string>(nullptr));
        }



    private:

        // swap function for exception safety / efficiency
        friend
        void swap(BMP& l, BMP& r)
        {
            using std::swap;

            swap(l.m_width, r.m_width);
            swap(l.m_height, r.m_height);
            swap(l.m_bit_count, r.m_bit_count);
            swap(l.m_width_pad, r.m_width_pad);
            swap(l.m_width_memory, r.m_width_memory);
            swap(l.m_data, r.m_data);
        }

        // TODO: WARNING: bit_count ONLY WORKS FOR 24, 32 BIT IMAGES! (due to / 8 operation)


    protected:

        void
        reinitialize(const LONG width, const LONG height, const WORD bit_count)
        {
            m_width = width;
            m_height = height;
            m_bit_count = bit_count;
            m_width_pad = (4 - ((LONG)(bit_count / 8) * width) % 4) % 4; // BYTES!
            m_width_memory = (LONG)(bit_count / 8) * width + m_width_pad; // BYTES!
            m_data.realloc(m_width_memory * m_height);
            //std::cout << "BMPBase: width=" << m_width << " height=" << m_height << " pad=" << m_width_pad << " width_mem=" << m_width_memory << std::endl;
        }


    public:

        //explicit
	    //BMPCanvas(const uint32_t x, const uint32_t y, const BMPFont* const bmpfont_ptr)
	    //	: m_data(3 * x * y) // enough data for x * y pixels, RGB format
	    //	, m_size_x{x}
	    //	, m_size_y{y}
	    //{
        //    // clear canvas
        //    DrawRect(0, 0, m_size_x - 1, m_size_y - 1, Color(255));
        //}


        // blank constructor required for loading from file (before properties are known)
        BMP()
            : m_width{0}
            , m_height{0}
            , m_bit_count{0}
            , m_width_pad{0}
            , m_width_memory{0}
            , m_data(0)
        {
        }


        // regular constructor
        BMP(const LONG width, const LONG height, const WORD bit_count)
            : m_width{width}
            , m_height{height}
            , m_bit_count{bit_count}
            , m_width_pad{(4 - ((LONG)(bit_count / 8) * width) % 4) % 4}
            , m_width_memory{(bit_count / 8) * width + m_width_pad}
            , m_data(m_width_memory * m_height)
            //: BMP(0, 0, 0) // this is to save duplicate code
        {
            //reinitialize(width, height, bit_count); // this is to save duplicate code
            //std::cout << "BMP: width=" << m_width << " height=" << m_height << " pad=" << m_width_pad << " width_mem=" << m_width_memory << std::endl;
        }


        // loader constructor (load from file)
        BMP(const std::string& filename)
            : BMP(0, 0, 0) // rather than inheriting, should this class have BMP as a data member?
        {
            Load(filename);
        }


        // blank constructor (create new file in memory)
        BMP(const LONG width, const LONG height, const WORD bit_count)
            : BMP(width, height, bit_count)
        {
            // do nothing, file created in memory
            //Clear(); // rgb clear?
        }


        virtual
	    ~BMP()
	    {
	        //std::cout << "BMP" << std::endl;
	    }


        // Copy constructor
	    BMP(const BMP& bmpsurface)
            : m_width{bmpsurface.m_width}
		    , m_height{bmpsurface.m_height}
		    , m_bit_count{bmpsurface.m_bit_count}
		    , m_width_pad{bmpsurface.m_width_pad}
		    , m_width_memory{bmpsurface.m_width_memory}
		    , m_data{bmpsurface.m_data}
	    {
	    }


        // Move constructor
	    BMP(BMP&& bmpsurface)
		    : BMP(0, 0, 0)
	    {
            swap(*this, bmpsurface);
        }


        // Copy and move assignment operator using swap() function
        // See MANUAL
	    BMP& operator=(BMP bmpsurface)
	    {
            swap(*this, bmpsurface);

		    return *this;
	    }


        virtual
        void Load(const std::string& filename)
        {
            std::string f_ext{filename_extension(filename)};

            if(f_ext == std::string("bmp"))
            {
                LoadBMP(filename); // leave .bmp on for argument
            }
            else if(f_ext == std::string("png"))
            {
                std::cerr << "TODO" << std::endl;
                //LoadPNG(filename); // leave .png on for argument
            }
            else
            {
                std::cerr << "Could not detect file type from filename extension. File not loaded!" << std::endl;
            }
        }



        // Automatic save as, detects file format depending on
        // extension to filename supplied.
        // eg, bmp format class,
        // png format class
        // and one which inherits from all which can save as all raster graphics formats
        virtual // future classes should over-ride this depending on what formats that class can save
        void SaveAs(const std::string filename) const
        {
            std::string f_ext{filename_extension(filename)};

            if(f_ext == std::string("bmp"))
            {
                SaveAsBitmap(filename); // leave .bmp on for argument
            }
            else if(f_ext == std::string("png"))
            {
                std::cerr << "TODO" << std::endl;
                //SaveAsPNG(filename); // leave .png on for argument
            }
            else
            {
                std::cerr << "Could not detect file type from filename extension. Defaulting to bitmap file format" << std::endl;
                SaveAsBitmap(filename); // leave .bmp on for argument
            }

        }


        void LoadBMP(const std::string& filename)
        {
            std::ifstream inputfile(filename.c_str(), std::ios::binary);
            if(!inputfile.is_open())
            {
                std::cerr << "Unable to open input file " << filename << std::endl;
            }
            else
            {
                BITMAPFILEHEADER f_head;
                //f_head.bfType = ((WORD)'B' << 0x08) | ((WORD)'M' << 0x00);
                //f_head.bfSize = uint_rev(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_bit_count * m_width_memory * m_height);
                //f_head.bfReserved1 = 0;
                //f_head.bfReserved2 = 0;
                //f_head.bfOffBits = uint_rev(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));


                // build standard bitmap file header
                BITMAPINFOHEADER i_head;
                //i_head.biSize = uint_rev(sizeof(BITMAPINFOHEADER));
                //i_head.biWidth = ulong_rev(m_width);
                //i_head.biHeight = ulong_rev(m_height);
                //i_head.biPlanes = ushort_rev(1);
                //i_head.biBitCount = ushort_rev(m_bit_count);
                //i_head.biCompression = 0;
                //i_head.biSizeImage = uint_rev(m_bit_count * m_width_memory * m_height);
                //i_head.biXPelsPerMeter = 0;
                //i_head.biYPelsPerMeter = 0;
                //i_head.biClrUsed = 0;
                //i_head.biClrImportant = 0;

                inputfile.read((char*)&f_head, sizeof(BITMAPFILEHEADER));
                inputfile.read((char*)&i_head, sizeof(BITMAPINFOHEADER));

                //std::cout << "BITMAPFILEHEADER: " << sizeof(BITMAPFILEHEADER) << std::endl;
                //std::cout << "BITMAPINFOHEADER: " << sizeof(BITMAPINFOHEADER) << std::endl;

                if(f_head.bfType == ushort_rev(((WORD)'B' << 0x08) | ((WORD)'M' << 0x00)))
                {
                    std::streampos /*size_t*/ index_temp{inputfile.tellg()};
                    inputfile.seekg(0, std::ios::end);
                    /*size_t*/ std::streampos file_size{inputfile.tellg()};
                    if(f_head.bfSize != file_size)
                    {
                        std::cerr << "File head error: Head file size label does not match file size." << std::endl;
                            //std::cerr << "f_head.bfSize=" << f_head.bfSize << std::endl;
                            //std::cerr << file_size << std::endl;
                            //std::cerr << file_size << std::endl;
                            //std::cout << ((f_head.bfSize & 0xff000000) >> 24) << std::endl;
                            //std::cout << ((f_head.bfSize & 0x00ff0000) >> 16) << std::endl;
                            //std::cout << ((f_head.bfSize & 0x0000ff00) >>  8) << std::endl;
                            //std::cout << ((f_head.bfSize & 0x000000ff) >>  0) << std::endl;
                    }
                    else
                    {
                        inputfile.seekg(index_temp, std::ios::beg);
                        if(f_head.bfReserved1 != 0)
                        {
                            std::cerr << "Warning: bfReserved1 non-zero, ignore" << std::endl;
                        }
                        // don't care about this result

                        if(f_head.bfReserved2 != 0)
                        {
                            std::cerr << "Warning: bfReserved2 non-zero, ignore" << std::endl;
                        }
                        // don't care about this result

                        // get offset
                        //size_t bitmap_data_offset{uint_rev(f_head.bfOffBits)};

                        if(i_head.biSize != sizeof(BITMAPINFOHEADER))
                        {
                            std::cerr << "File info head error: Unexpected info head size value" << std::endl;
                                std::cerr << sizeof(BITMAPINFOHEADER) << std::endl;
                                std::cerr << i_head.biSize << std::endl;
                        }
                        else
                        {
                            if(i_head.biPlanes != 1)
                            {
                                std::cerr << "File info head error: Unexpected info head planes value" << std::endl;
                            }
                            else
                            {
                                if(i_head.biBitCount != 24)
                                {
                                    std::cerr << "File info head error: Unexpected info head bit count value" << std::endl;
                                }
                                else
                                {
                                    size_t expected_pad{(4 - ((LONG)(i_head.biBitCount / 8) * i_head.biWidth) % 4) % 4};
                                    size_t expected_width_memory{((i_head.biBitCount / 8) * i_head.biWidth) + expected_pad};
                                    size_t expected_size{expected_width_memory * i_head.biHeight};
                                    if(file_size != expected_size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
                                    {
                                        std::cerr << "File info head error: Calculated file size does not match value calculated from header size, info header size, image width, height, depth." << std::endl;
                                        //std::cerr << file_size << std::endl;
                                        //std::cerr << expected_size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) << std::endl;
                                        //std::cerr << i_head.biWidth << ", " << i_head.biHeight << ", " << expected_pad << std::endl;
                                        // TODO: leave these debug statements in
                                    }
                                    else
                                    {
                                        if(i_head.biCompression != 0)
                                        {
                                            std::cerr << "Image is compressed, load abort" << std::endl;
                                        }
                                        else
                                        {
                                            if(i_head.biSizeImage != expected_size)
                                            {
                                                std::cerr << "File info head error: Calculated file size does not match value calculated from image width, height, depth." << std::endl;
                                            }
                                            else
                                            {
                                                if(i_head.biXPelsPerMeter)
                                                {
                                                    #ifdef WARNINGS_ON
                                                    std::cerr << "Warning: Ignoring non-zero value for biXPelsPerMeter" << std::endl;
                                                    #endif
                                                }

                                                if(i_head.biYPelsPerMeter)
                                                {
                                                    #ifdef WARNINGS_ON
                                                    std::cerr << "Warning: Ignoring non-zero value for biYPelsPerMeter" << std::endl;
                                                    #endif
                                                }

                                                if(i_head.biClrUsed)
                                                {
                                                    std::cerr << "Warning: Ignoring non-zero value for color pallet, used" << std::endl;
                                                }

                                                if(i_head.biClrImportant)
                                                {
                                                    std::cerr << "Warning: Ignoring non-zero value for color pallet, important" << std::endl;
                                                }

                                                // init memory
                                                reinitialize(i_head.biWidth, i_head.biHeight, i_head.biBitCount);

                                                // load memory
                                                inputfile.seekg(f_head.bfOffBits);
                                                //std::cout << "SEEK: " << f_head.bfOffBits << std::endl;

                                                // read data
                                                for(unsigned int y = 0; y < m_height; ++ y)
                                                {
                                                    inputfile.read((char*)(&m_data[y * m_width_memory]), m_width_memory);

                                                    // TODO: need to add a m_size_x_pad variable and m_x_pad variable, and include the padding in memory
                                                    // don't bother putting zeros for padding, just write whatever is in the memory array
                                                    //for(unsigned int p = 0; p < x_pad; ++ p)
                                                    //	outputfile.put(0); // put as many zeros required for padding
                                                }

                                                //outputfile.flush();
                                                inputfile.close();

                                                //std::clog << "Canvas read from input file " << filename << std::endl;


                                            } // biSizeImage
                                        } // biCompression

                                    } // file_size


                                } // biBitCount
                            } // biPlanes
                        } // biSize

                    } // f_head.bfSize
                } // f_head.bfType
                else
                {
                    std::cerr << "File format error: Missing 'B'|'M' from head." << std::endl;
                }
            }
        }


        // Saves data in array to file with correctly formatted
        // bitmap file header.
        void SaveAsBitmap(const std::string filename) const
        {
            std::ofstream outputfile(filename.c_str(), std::ios::binary);
            //outputfile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            //try
            //{
            //    outputfile.open(filename.c_str(), std::ios::binary);
           //}
            //catch(std::ofstream::failure ofstreamexcept)
            //{
            //    //ofstreamexcept.
            //    //std::ios_base::failure& e
            //    std::cerr << ofstreamexcept.what() << std::endl;
            //}

            if(!outputfile.is_open())
            {
                std::cerr << "Unable to open output file " << filename << std::endl;
            }
            else
            {
                // compute the amount of padding required - this is because the bitmap file
                // format must end on a 4 byte boundry for every line of pixels
                //unsigned short x_pad = (4 - (3 * m_size_x) % 4) % 4;
                //if((3 * m_size_x) % 4 == 0)
                //{
                //	x_pad = 0;
                //}
                //else
                //{
                //	x_pad = 4 - (3 * m_size_x) % 4;
                //}

                BITMAPFILEHEADER f_head;
                f_head.bfType = ushort_rev(((WORD)'B' << 0x08) | ((WORD)'M' << 0x00));
                f_head.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_width_memory * m_height; //m_bit_count *
                f_head.bfReserved1 = 0;
                f_head.bfReserved2 = 0;
                f_head.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


                // build standard bitmap file header
                BITMAPINFOHEADER i_head;
                i_head.biSize = sizeof(BITMAPINFOHEADER);
                i_head.biWidth = m_width;
                i_head.biHeight = m_height;
                i_head.biPlanes = 1;
                i_head.biBitCount = m_bit_count;
                i_head.biCompression = 0;
                i_head.biSizeImage = m_width_memory * m_height;
                i_head.biXPelsPerMeter = 0;
                i_head.biYPelsPerMeter = 0;
                i_head.biClrUsed = 0;
                i_head.biClrImportant = 0;

                outputfile.write((char*)&f_head, sizeof(BITMAPFILEHEADER));
                outputfile.write((char*)&i_head, sizeof(BITMAPINFOHEADER));

            /*
                // build the standard bitmap file header
                unsigned char h[54];

                // BMP Header
                h[0] = 'B'; h[1] = 'M'; // BM - these characters have to be here
                int_to_char_array_reversed(14 + 40 + (3 * m_size_x + x_pad) * m_size_y, &h[2]); // Put total filesize into h[2], h[3], h[4], h[5]
                h[6] = 0x00; h[7] = 0x00; h[8] = 0x00; h[9] = 0x00; // Reserved bytes, set to zero
                h[10] = 0x36; h[11] = 0x00; h[12] = 0x00; h[13] = 0x00; // Pixel data offset, always starts at 0x36 == 54

                // DIP Header
                h[14] = 0x28; h[15] = 0x00; h[16] = 0x00; h[17] = 0x00; // Size of DIP header, endian reverse, 0x28 == 40
                int_to_char_array_reversed(m_size_x, &h[18]); // Width of image (signed int)
                int_to_char_array_reversed(m_size_y, &h[22]); // Height of image (signed int)
                h[26] = 0x01; h[27] = 0x00; // Number of planes (1)
                h[28] = 0x18; h[29] = 0x00; // 24 bits per pixel, 0x18 == 24
                h[30] = 0x00; h[31] = 0x00; h[32] = 0x00; h[33] = 0x00; // Compression method (none)
                int_to_char_array_reversed((3 * m_size_x + x_pad) * m_size_y, &h[34]); // Image size - same as file size - 54 bytes for header
                for(int ix = 38; ix < 29 + 4 * 6; ++ ix) // X Res, Y Res, Colors in color pallet, More info on color pallet - just set all of this stuff to zero
                    h[ix] = 0x00; // 2016-04-21 Not sure why the hell I chose 29 + 4 * 6 but it doesnt matter if array initialized to zero

                // 29 + 4 * 6 = 29 + 24 = 53 (should be <= )
                // 54 - 38 = 16

                // write header to file
                outputfile.write((char*)h, 54);
            */


                // write data
                for(unsigned int y = 0; y < m_height; ++ y)
                {
                    //outputfile.write((char*)(m_data + y * (3 * m_size_x)), 3 * m_size_x);
                    outputfile.write((char*)(&m_data[y * m_width_memory]), m_width_memory);

                    // TODO: need to add a m_size_x_pad variable and m_x_pad variable, and include the padding in memory
                    // don't bother putting zeros for padding, just write whatever is in the memory array
                    //for(unsigned int p = 0; p < x_pad; ++ p)
                    //	outputfile.put(0); // put as many zeros required for padding
                }

                outputfile.flush();
                outputfile.close();

                //std::clog << "Canvas written to output file " << filename << std::endl;
            }
        }


    };




}

#endif
