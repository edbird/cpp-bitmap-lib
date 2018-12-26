#include "bitmap.hpp"


// SFML headers
#include <SFML/Graphics.hpp>


// C++ headers
#include <string>
#include <iostream>
#include <fstream>


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

    b_g.Resize(640, 480);
    b_g.SaveAs("img_out_g_resize.bmp");
    b_g.Resize(800, 600);
    b_g.SaveAs("img_out_g_resize_2.bmp");

    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // LOAD SFML
    ////////////////////////////////////////////////////////////////////////////

    // init SFML

    sf::RenderWindow window(sf::VideoMode(800, 600), "cpp-bitmap-lib", sf::Style::Titlebar | sf::Style::Close);

    sf::Texture texture;
    if(!texture.loadFromFile("img_out_g_resize.bmp"))
    {
        std::cerr << "Texture load failure: " << std::endl;
    }

    // program main window
    while(window.isOpen())
    {
        
        // event loop
        sf::Event event;

        while(window.pollEvent(event))
        {
            
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            /*else if(event.type == sf::EventResized)
            {

            }*/
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::F1 || event.key.code == sf::Keyboard::F2)
                {
                    if(event.key.code == sf::Keyboard::F1)
                    {
                        if(!texture.loadFromFile("img_out_g_resize.bmp"))
                        {
                            std::cerr << "Texture load failure: " << std::endl;
                        }
                        std::cout << "F1" << std::endl;
                    }
                    else if(event.key.code == sf::Keyboard::F2)
                    {
                        if(!texture.loadFromFile("img_out_g_resize_2.bmp"))
                        {
                            std::cerr << "Texture load failure: " << std::endl;
                        }
                        std::cout << "F2" << std::endl;
                    }

                    if(window.getSize().x != texture.getSize().x)
                    {
                        if(window.getSize().y != texture.getSize().y)
                        {
                            window.setView(sf::View(sf::FloatRect(0.0, 0.0, texture.getSize().x, texture.getSize().y)));
                            window.setSize(sf::Vector2u(texture.getSize().x, texture.getSize().y));
                        }
                    }
                }
            }
                



        }


        window.clear(sf::Color::Black);

        //texture.create(b.Width(), b.Height());
        // requires RGBA

        /*
        if(!texture.loadFromMemory(b_g.SaveMem().data(), b_g.SaveMem().size()))
        {
            std::cerr << "Texture load failure: " << std::endl;
        }

        if(!texture.loadFromFile("img_out_g_resize.bmp"))
        {
            std::cerr << "Texture load failure: " << std::endl;
        }
        */

        sf::Sprite sprite;
        sprite.setTexture(texture);

        window.draw(sprite);

        window.display();

    }

    return 0;

}
