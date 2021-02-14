#include <iostream>
#include <cstdlib> // For random numbers generation
#include <ctime>
#include "chip8.h"

void Chip8::initialize(){
	// program counter starts at 0x200
	pc       = 0x200;
	opcode   = 0;
	I	     = 0;

	//std::srand(std::time(nullptr));
	std::srand(0);

	// Clear display
	for(auto& c: gfx){
        c = 0;
	}

	// Clear keys
	for(auto& k: key){
        k = 0;
	}

	// Clear stack and stack pointer
	for(auto& p: stack){
        p = 0;
	}
	sp = 0;

	// Clear registers
	for(auto& v: V){
        v = 0;
	}

	// Clear memory
	for(auto& mem: memory){
        mem = 0;
	}

	// Load fontset
	for(int i=0; i<80; i++){
		memory[i] = chip8_fontset[i];
	}

	// reset timers
	delay_timer = 0;
    sound_timer = 0;

    drawFlag = false;
}

void Chip8::emulateCycle(){
	// Fetch opcode
	opcode = (memory[pc] << 8) | (memory[pc+1]);

	// Precalculate opcode bits
	unsigned short X   = (opcode & 0x0F00) >> 8;
	unsigned short Y   = (opcode & 0x00F0) >> 4;
	unsigned short NNN = opcode & 0x0FFF;
	unsigned short  NN = opcode & 0x00FF;

	// Decode opcode
	switch(opcode & 0xF000)
    {
        // OP Codes with MSB 0
        case 0x0000:
            switch(opcode & 0x000F)
            {
                // 0x00E0 : Clears the screen.
                case 0x0000:
                    for(auto& p: gfx){ p = 0; }
                    pc += 2;
                    //op_clearScreen(opcode);
                    break;

                // 0x00EE : Returns from a subroutine
                case 0x000E:
                    --sp;
                    pc = stack[sp] + 2;
                    break;

                default:
                    std::cout <<"Unknown opcode " << opcode << std::endl;
            }
            break;

        // 0x1NNN : Jump to Address NNN
        case 0x1000:
            pc = NNN;
            break;

        // 0x2NNN : Call subroutine at NNN
        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = NNN;
            break;

        // 0x3XNN : Skip next instruction if VX == NN
        case 0x3000:
            if(V[X] == NN) { pc += 4; }
            else { pc += 2; }
            break;

        // 0x4XNN : Skip next instruction if VX != NN
        case 0x4000:
            if(V[X] != NN) { pc += 4; }
            else { pc += 2; }
            break;

        // 0x5XY0 : Skips the next instruction if VX equals VY.
        case 0x5000:
            if(V[X] == V[Y]) { pc += 4; }
            else { pc += 2; }
            break;

        // 0x6XNN : Sets VX to NN
        case 0x6000:
            V[X] = NN;
            pc += 2;
            break;

        // 0x7XNN : Adds NN to VX (Carry flag is not changed)
        case 0x7000:
            V[X] += NN;
            pc += 2;
            break;

        // OP Codes with MSB 8
        case 0x8000:
            switch(opcode & 0x000F)
            {
                // 0x8XY0 : Sets VX to the value of VY
                case 0x0000:
                    V[X] = V[Y];
                    pc += 2;
                    break;

                // 0x8XY1 : Sets VX to VX or VY (Bitwise OR operation)
                case 0x0001:
                    V[X] = V[X] | V[Y];
                    pc += 2;
                    break;

                // 0x8XY2 : Sets VX to VX and VY (Bitwise AND operation)
                case 0x0002:
                    V[X] = V[X] & V[Y];
                    pc += 2;
                    break;

                // 0x8XY3 : Sets VX to VX xor VY (Bitwise XOR operation)
                case 0x0003:
                    V[X] = V[X] ^ V[Y];
                    pc += 2;
                    break;

                // 0x8XY4 : Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                case 0x0004:
                    if(V[Y] > (0xFF - V[X])){ V[0xF] = 1; }
                    else { V[0xF] = 0; }
                    V[X] += V[Y];
                    pc += 2;
                    break;

                // 0x8XY5 : VY is subtracted from VX.
                // VF is set to 0 when there's a borrow, and 1 when there isn't.
                case 0x0005:
                    if(V[Y] > V[X]){ V[0xF] = 0; }
                    else{ V[0xF] = 1; }
                    V[X] = V[X] - V[Y];
                    pc += 2;
                    break;

                // 0x8XY6 : Stores the least significant bit of VX in VF and then shifts VX to the right by 1
                case 0x0006:
                    V[0xF] = V[X] & 0x1;
                    V[X] >>= 1;
                    pc += 2;
                    break;

                // 0x8XY7 : Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                case 0x0007:
                    if(V[X] > V[Y]) { V[0xF] = 0; }
                    else { V[0xF] = 1; }
                    V[X] = V[Y] - V[X];
                    pc += 2;
                    break;

                // 0x8XYE : Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                case 0x000E:
                    V[0xF] = (V[X] & 0x80) >> 7;
                    V[X] <<= 1;
                    pc += 2;
                    break;

                default:
                    std::cout <<"Unknown opcode " << opcode << std::endl;
            }
            break;


        // 0x9XY0 : Skips the next instruction if VX doesn't equal VY.
        case 0x9000:
            if(V[X] != V[Y]){ pc += 4; }
            else { pc += 2; }
            break;

        // 0xANNN : Sets I to the address NNN.
        case 0xA000:
            I = NNN;
            pc += 2;
            break;

        // 0xBNNN : Jumps to the address NNN plus V0.
        case 0xB000:
            pc = NNN + V[0];
            break;

        // 0xCXNN : Sets VX to the result of a bitwise and operation on a random
        // number (Typically: 0 to 255) and NN.
        case 0xC000:
            V[X] = std::rand() & NN;
            pc += 2;
            break;

        // 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of
        // N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory location I;
        // I value doesn’t change after the execution of this instruction.
        // As described above, VF is set to 1 if any screen pixels are flipped from set to unset
        // when the sprite is drawn, and to 0 if that doesn’t happen
        case 0xD000: // 0xDXYN
            op_drawSpriteAtCoordVXVY(opcode);
            break;

        // OP Codes with MSB E
        case 0xE000:
            switch(opcode & 0x000F)
            {
                // 0xEX9E : Skips the next instruction if the key stored in VX is pressed.
                case 0x000E:
                    if(key[V[X]] != 0){ pc += 4; }
                    else { pc += 2; }
                    break;

                // 0xEXA1 : Skips the next instruction if the key stored in VX isn't pressed.
                case 0x0001:
                    if(key[V[X]] == 0){ pc += 4; }
                    else { pc += 2; }
                    break;

                default:
                    std::cout <<"Unknown opcode " << opcode << std::endl;
            }
            break;

        // OP Codes with MSB F
        case 0xF000:
            switch(opcode & 0x000F)
            {
                // 0xFX07 : Sets VX to the value of the delay timer.
                case 0x0007:
                    V[X] = delay_timer;
                    pc += 2;
                    break;

                // 0xFX0A : A key press is awaited, and then stored in VX.
                // (Blocking Operation. All instruction halted until next key event)
                case 0x000A:
                    for(unsigned int i=0; i<16; i++){
                        if(key[i] != 0){
                            V[X] = i;
                            pc += 2;
                            break;
                        }
                    }
                    break;

                // 0xFX18 : Sets the sound timer to VX.
                case 0x0008:
                    sound_timer = V[X];
                    pc += 2;
                    break;

                // 0xFX1E : Adds VX to I. VF is not affected.
                case 0x000E:
                    I += V[X];
                    pc += 2;
                    break;

                // 0xFX29 : Sets I to the location of the sprite for the character in VX.
                // Characters 0-F (in hexadecimal) are represented by a 4x5 font
                case 0x0009:
                    I = 5*V[X];
                    pc += 2;
                    break;

                // 0xFX33 : Stores the binary-coded decimal representation of VX, with the most
                // significant of three digits at the address in I, the middle digit at I plus 1,
                // and the least significant digit at I plus 2.
                case 0x0003:
                    op_storeBcdRepOfVxAtI0To2(opcode);
                    break;

                case 0x0005:
                    switch(opcode & 0x00F0){
                        // 0xFX15 : Sets the delay timer to VX.
                        case 0x0010:
                            delay_timer = V[X];
                            pc += 2;
                            break;

                        // 0xFX55 : Stores V0 to VX (including VX) in memory starting at
                        // address I. The offset from I is increased by 1 for each value written,
                        // but I itself is left unmodified.
                        case 0x0050:
                            for(unsigned int i=0; i <= X; i++){ memory[I+i] = V[i]; }
                            pc += 2;
                            break;

                        // 0xFX65 : Fills V0 to VX (including VX) with values from memory starting
                        // at address I. The offset from I is increased by 1 for each value
                        // written, but I itself is left unmodified.
                        case 0x0060:
                            for(unsigned int i=0; i <= X; i++){ V[i] = memory[i+I]; }
                            pc += 2;
                            break;
                    }
                    break;
            }
            break;
    }

	// Execute opcode
	// update timers
    if(delay_timer > 0)
        --delay_timer;

    if(sound_timer > 0)
    {
        if(sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void Chip8::printStatus(){
    std::cout << "------- Chip 8 Status: -------" << std::endl;
    std::cout << "    PC : " << std::hex << pc                 << std::endl;
    std::cout << "opcode : " << std::hex << opcode             << std::endl;
    std::cout << " stack : " << stack[0] << "," << stack[1] << "," << stack[2] << "," << stack[3] << ","
                             << stack[4] << "," << stack[5] << "," << stack[6] << "," << stack[7] << ","
                             << stack[8] << "," << stack[9] << "," << stack[10] << "," << stack[11] << std::endl;
    std::cout << "    SP : " << std::hex << sp                 << std::endl;
    std::cout << "     I : " << std::hex << (unsigned int)I    << std::endl;
    std::cout << "dly tmr: " << std::dec << (unsigned int)delay_timer << std::endl;
    std::cout << "  V[0] : " << std::hex << (unsigned int)V[0] << std::endl;
    std::cout << "  V[1] : " << std::hex << (unsigned int)V[1] << std::endl;
    std::cout << "  V[2] : " << std::hex << (unsigned int)V[2] << std::endl;
    std::cout << "  V[3] : " << std::hex << (unsigned int)V[3] << std::endl;
    std::cout << "  V[4] : " << std::hex << (unsigned int)V[4] << std::endl;
    std::cout << "  V[5] : " << std::hex << (unsigned int)V[5] << std::endl;
    std::cout << "  V[6] : " << std::hex << (unsigned int)V[6] << std::endl;
    std::cout << "  V[7] : " << std::hex << (unsigned int)V[7] << std::endl;
    std::cout << "  V[8] : " << std::hex << (unsigned int)V[8] << std::endl;
    std::cout << "  V[9] : " << std::hex << (unsigned int)V[9] << std::endl;
    std::cout << "  V[a] : " << std::hex << (unsigned int)V[0xa] << std::endl;
    std::cout << "  V[b] : " << std::hex << (unsigned int)V[0xb] << std::endl;
    std::cout << "  V[c] : " << std::hex << (unsigned int)V[0xc] << std::endl;
    std::cout << "  V[d] : " << std::hex << (unsigned int)V[0xd] << std::endl;
    std::cout << "  V[e] : " << std::hex << (unsigned int)V[0xe] << std::endl;
    std::cout << "  V[f] : " << std::hex << (unsigned int)V[0xf] << std::endl;

}

void Chip8::load(){
    std::cout << "------- Loading Game: " << filename << " -------" << std::endl;

    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen ( filename , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */


    // transfer the contents of buffer to the memory starting
    // at address 0x200
    for(int i = 0; i < lSize; ++i)
    memory[i + 512] = buffer[i];

    // terminate
    fclose (pFile);
    free(buffer);
}

void Chip8::op_drawSpriteAtCoordVXVY(unsigned short code){
    unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for(int xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {
                if(gfx[(x + xline + ((y + yline) * 64))] == 1){
                    V[0xF] = 1;
                }
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    drawFlag = true;
    pc += 2;
}

void Chip8::op_storeBcdRepOfVxAtI0To2(unsigned short code){
    unsigned short x  = (code & 0x0F00) >> 8;

    memory[I]     =  V[x] / 100;
    memory[I + 1] = (V[x] / 10 )  % 10;
    memory[I + 2] = (V[x] % 100) % 10;
    pc += 2;
}

void Chip8::copyGfxBuffer(unsigned char* targetBuffer){
    for(unsigned int i=0; i<64*32; i++){
        targetBuffer[i] = gfx[i];
    }
}

void Chip8::copyKeyBuffer(unsigned char* sourceKey){
    for(unsigned int i=0; i<16; i++){
        key[i] = sourceKey[i];
    }
}

void Chip8::setGameFileName(char* l_filename){
    filename = l_filename;
}

void Chip8::resetGame(){
    initialize();
    load();
}

//
// EOF
//
