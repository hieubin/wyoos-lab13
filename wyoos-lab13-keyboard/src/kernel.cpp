#include "types.h"
#include "gdt.h"
#include "interrupts.h"

void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint8_t x = 0, y = 0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            case '\r':
                x = 0;
                break;
            case '\b':
                if(x > 0) {
                    x--;
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printNum(int32_t num)
{
    if(num == 0)
    {
        printf("0");
        return;
    }
    
    if(num < 0)
    {
        printf("-");
        num = -num;
    }
    
    char buffer[16];
    int pos = 0;
    
    while(num > 0)
    {
        buffer[pos] = '0' + (num % 10);
        num = num / 10;
        pos++;
    }
    
    for(int i = pos - 1; i >= 0; i--)
    {
        char temp[2];
        temp[0] = buffer[i];
        temp[1] = '\0';
        printf(temp);
    }
}

void StartQuadraticSolver();

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(void* multibootStruct, uint32_t /*magicNumber*/)
{
    GlobalDescriptorTable gdt;
    
    InterruptManager interrupts(0x20, &gdt);
    
    printf("Initializing system...\n");
    
    interrupts.Activate();
    
    StartQuadraticSolver();

    while(1);
}
