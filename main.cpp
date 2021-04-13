#include <stdio.h>

// http://www.obelusk.me.uk/6502/
using Byte = unsigned  char;
using Word = unsigned short;
using u32 = unsigned int;
struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise()
    {
        for(auto &block:Data){
            block=0;
        }
    }
    // read one byte
    Byte operator [](u32 Address) const{

        // assert here Address is < Max_MEM
        return Data[Address];
    }
    // write one byte
    Byte& operator [](u32 Address) {

        // assert here Address is < Max_MEM
        return Data[Address];
    }
};

struct CPU
{

    Word PC;        //Program counter
    Word SP;        //Stack pointer
    Byte A,X,Y;     //Registers
    Byte C : 1;     //Status flags
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    void Reset(Mem& memory){

        PC = 0xFFFC;
        SP = 0x0100;
        D = 0;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;

        memory.Initialise();
    }
    // doesn't increment the program counter because no code execution is done
    Byte ReadByte(Byte Address,u32& Cycles, Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    void LDASetStatus()
    {
        Z = (A==0);
        N = (A & 0b10000000 ) > 0;
    }

    // opcodes
    static constexpr Byte INS_LDA_IM = 0xA9;
    static constexpr Byte INS_LDA_ZP = 0xA5;

    void Execute(u32  Cycles,Mem& memory) {
        while(Cycles >0)
        {
            // fetch next instruction
            Byte Ins=FetchByte(Cycles,memory);
            switch(Ins){
                case INS_LDA_IM:
                {
                    Byte Value = FetchByte(Cycles,memory);
                    A = Value;
                    LDASetStatus();
                }break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles,memory);
                    A = ReadByte(ZeroPageAddress,Cycles,memory);
                    LDASetStatus();
                }break;
                default:
                    printf("Instruction not handled %d",Ins);
                    break;

            }
        }
    }
    Byte FetchByte(u32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC++];
        Cycles--;
        return Data;
    }
};


int main() {
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    // start - inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD]= 0x42;
    mem[0x0042]= 0x84;
    // end - inline a little program
    cpu.Execute(3,mem);
    return 0;
}
