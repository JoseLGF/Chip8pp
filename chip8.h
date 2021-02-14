/*
 * File: chip8.h
 * Description: This file contains the header definitions for the chip8 class.
 * */

 #include <string>

class Chip8 {
public:

    /* Public interface */
	void initialize();
	void emulateCycle();
	void printStatus();
	void load();
	void copyGfxBuffer(unsigned char* targetBuffer);
	void copyKeyBuffer(unsigned char* sourceKey);
	void setGameFileName(char* filename);
	void resetGame();

	// Special OpCode operations
	void op_drawSpriteAtCoordVXVY(unsigned short code);
	void op_storeBcdRepOfVxAtI0To2(unsigned short code);

    // There are 35 opcodes, all of them two bytes long.
    // Stores the current opcode.
    unsigned short opcode;

    // The chip 8 contains 4k bytes of memory
    // Memory map in Chip8:
    // 0x000-0x1FF - Chip 8 Interpreter (font set in emu)
    // 0x050-0x0A0 - Used for the built-in 4x5 pixel font set (0-F)
    // 0x200-0xFFF - Program ROM and work RAM
    unsigned char memory[4096];

    // The chip8 contains 15 8-bit general purpose registers
    // named V0-VE. The 16th register is used for the 'carry' flag.
    unsigned char V[16];

    // Index registers and program counter
    // Their value can range from 0x000 to 0xFFF
    unsigned short I;
    unsigned short pc;

    // The graphics in the chip8 are black and white and the screen has a total
    // of 2048 pixels (64*32)
    unsigned char gfx[64*32];

    // The draw flag indicates that we want to write to the screen
    bool drawFlag;

    // There are no interrupts or hardware registers, but there are two timer
    // registers that count at 60Hz. When set above 0 they will count to 0.
    unsigned char delay_timer;
    // The system's buzzer sounds whenever the sound timer reaches 0.
    unsigned char sound_timer;

    // function call stack with depth 16
    unsigned short stack[16];
    // The stack pointer.
    unsigned short sp;

    // the chip8 uses a hex keypad as input method.
    unsigned char key[16];

    // chip8_fontset
    unsigned char chip8_fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // The name of a chip8 game
    char* filename;
};

//
// EOF
//
