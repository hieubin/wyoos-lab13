#include "port.h"

Port::Port(uint16_t portnum)
{
    portnumber = portnum;
}

Port::~Port()
{
}

Port8Bit::Port8Bit(uint16_t portnum)
    : Port(portnum)
{
}

Port8Bit::~Port8Bit()
{
}

void Port8Bit::Write(uint8_t data)
{
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (portnumber));
}

uint8_t Port8Bit::Read()
{
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (portnumber));
    return result;
}

Port8BitSlow::Port8BitSlow(uint16_t portnum)
    : Port8Bit(portnum)
{
}

Port8BitSlow::~Port8BitSlow()
{
}

void Port8BitSlow::Write(uint8_t data)
{
    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (portnumber));
}
