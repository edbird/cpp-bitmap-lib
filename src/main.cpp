#include "bitmap.hpp"


int main()
{

    BMP::BITMAP b("img.bmp");
    BMP::BITMAP b_r(b);
    BMP::BITMAP b_g(b);
    BMP::BITMAP b_b(b);

    // filter color channels
    b_r.RGBFilterAND(0xFF, 0x00, 0x00);
    b_g.RGBFilterAND(0x00, 0xFF, 0x00);
    b_b.RGBFilterAND(0x00, 0x00, 0xFF);

    b_r.Translate(-10, -5);
    b_b.Translate(10, 0);

    b_g.OR(b_r);
    b_g.OR(b_b);

    b.SaveAs("img_out.bmp");
    
    b_r.SaveAs("img_out_r.bmp");
    b_g.SaveAs("img_out_g.bmp");
    b_b.SaveAs("img_out_b.bmp");

    return 0;

}
