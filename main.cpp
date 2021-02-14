//
// This is the main file for the chip8 emulator program.
//

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include "chip8.h"
#include "config.h"

void captureInputs(sf::RenderWindow& window, Chip8& myChip8, unsigned char* keys){

    // Emulation speed is (roughly) the amount of instructions executed per second
    static unsigned int emulationSpeed = 1000;

    unsigned char A = 0xA;
    unsigned char B = 0xB;
    unsigned char C = 0xC;
    unsigned char D = 0xD;
    unsigned char E = 0xE;
    unsigned char F = 0xF;

    using sf::Keyboard;

    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed){
            window.close();
        }

        // Left and Right arrows to decrease or increase emulation speed
        if (event.type == sf::Event::KeyPressed){
            if (Keyboard::isKeyPressed(Keyboard::Right)){
                emulationSpeed += 50;
                std::cout << "Set emulation speed to " << emulationSpeed << std::endl;
                window.setFramerateLimit(emulationSpeed);
            }
            if (Keyboard::isKeyPressed(Keyboard::Left)){
                emulationSpeed -= 50;
                std::cout << "Set emulation speed to " << emulationSpeed << std::endl;
                window.setFramerateLimit(emulationSpeed);
            }
        }

        // Enter to reset the game
        if (event.type == sf::Event::KeyPressed){
            if (Keyboard::isKeyPressed(Keyboard::Enter)){
                myChip8.resetGame();
            }
        }

        // Inputs for the Chip8
        keys[1] = Keyboard::isKeyPressed(Keyboard::Num1) ? 1 : 0;
        keys[2] = Keyboard::isKeyPressed(Keyboard::Num1) ? 1 : 0;
        keys[3] = Keyboard::isKeyPressed(Keyboard::Num1) ? 1 : 0;
        keys[C] = Keyboard::isKeyPressed(Keyboard::Num4) ? 1 : 0;
        keys[4] = Keyboard::isKeyPressed(Keyboard::Q)    ? 1 : 0;
        keys[5] = Keyboard::isKeyPressed(Keyboard::W)    ? 1 : 0;
        keys[6] = Keyboard::isKeyPressed(Keyboard::E)    ? 1 : 0;
        keys[D] = Keyboard::isKeyPressed(Keyboard::R)    ? 1 : 0;
        keys[7] = Keyboard::isKeyPressed(Keyboard::A)    ? 1 : 0;
        keys[8] = Keyboard::isKeyPressed(Keyboard::S)    ? 1 : 0;
        keys[9] = Keyboard::isKeyPressed(Keyboard::D)    ? 1 : 0;
        keys[E] = Keyboard::isKeyPressed(Keyboard::F)    ? 1 : 0;
        keys[A] = Keyboard::isKeyPressed(Keyboard::Z)    ? 1 : 0;
        keys[0] = Keyboard::isKeyPressed(Keyboard::X)    ? 1 : 0;
        keys[B] = Keyboard::isKeyPressed(Keyboard::C)    ? 1 : 0;
        keys[F] = Keyboard::isKeyPressed(Keyboard::V)    ? 1 : 0;
    }

    myChip8.copyKeyBuffer(keys);
}

void drawGraphics(Chip8& myChip8, sf::RenderWindow& window){
    // Create a 64x32 image filled with black color
    sf::Image image;
    image.create(64*config_DotSize, 64*config_DotSize, sf::Color::Black);

    unsigned char gfx_bfr[64*32];
    myChip8.copyGfxBuffer(gfx_bfr);

    // Draw the Chip8 pixels from the gfx buffer into the window screen
    for(int i=0; i<64; i++){
        for(int j=0; j<32; j++){
            bool thisPixel = gfx_bfr[i+j*64];
            sf::Color thisColor;

            // retrieve the current pixel color
            if(thisPixel){
                thisColor = sf::Color::White;
            }
            else{
                thisColor = sf::Color::Black;
            }

            // Draw the corresponding pixel
            for(int ii=0; ii<config_DotSize; ii++){
            for(int jj=0; jj<config_DotSize; jj++){
                image.setPixel(config_DotSize*i+ii, config_DotSize*j+jj, thisColor);
            }
            }

        }
    }

    // load image to texture
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite;
    sprite.setPosition(0, 0);
    sprite.setTexture(texture, false);

    // draw image in window
    window.draw(sprite);

}

int main(int argc, char** argv){

    // inputs keys (the chip8 uses a 16-button keypad)
    unsigned char keys[16]{};


    if(argc != 2){
        std::cout << "Error. Please provide a game name." << std::endl;
        //return 0;
    }

    // Setup chip8
	Chip8 myChip8;
	myChip8.initialize();
	char* fileName = argv[1];
	myChip8.setGameFileName(fileName);
	myChip8.load();

	// Setup graphics
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(64*config_DotSize, 32*config_DotSize), "Chip-8 Emulator", sf::Style::Default, settings);
    window.setFramerateLimit(1000);

    // Emulation loop
	while (window.isOpen())
    {
		myChip8.emulateCycle();

        // Draw to the screen
        window.clear();
        // if the draw flag is set, update the screen
		if(myChip8.drawFlag){
            drawGraphics(myChip8, window);
            window.display();
		}

        // Store the key press state
		captureInputs(window, myChip8, keys);
    }
}

//
// EOF
//
