#include "interrupts.h"

void printf(const char* str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

void InterruptManager::SetInterruptDescriptorTableEntry(uint8_t interrupt,
    uint16_t CodeSegmentSelectorOffset, void (*handler)(),
    uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType)
{
    interruptDescriptorTable[interrupt].handlerAddressLowBits = ((uint32_t) handler) & 0xFFFF;
    interruptDescriptorTable[interrupt].handlerAddressHighBits = (((uint32_t) handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interrupt].gdtCodeSegmentSelector = CodeSegmentSelectorOffset;
    
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interrupt].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
    interruptDescriptorTable[interrupt].reserved = 0;
}

InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* globalDescriptorTable)
    : programmableInterruptControllerMasterCommandPort(0x20),
      programmableInterruptControllerMasterDataPort(0x21),
      programmableInterruptControllerSlaveCommandPort(0xA0),
      programmableInterruptControllerSlaveDataPort(0xA1)
{
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint32_t CodeSegment = globalDescriptorTable->CodeSegmentSelector();

    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    for(uint8_t i = 255; i > 0; --i)
    {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }
    SetInterruptDescriptorTableEntry(0, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);

    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    programmableInterruptControllerMasterCommandPort.Write(0x11);
    programmableInterruptControllerSlaveCommandPort.Write(0x11);

    programmableInterruptControllerMasterDataPort.Write(hardwareInterruptOffset);
    programmableInterruptControllerSlaveDataPort.Write(hardwareInterruptOffset+8);

    programmableInterruptControllerMasterDataPort.Write(0x04);
    programmableInterruptControllerSlaveDataPort.Write(0x02);

    programmableInterruptControllerMasterDataPort.Write(0x01);
    programmableInterruptControllerSlaveDataPort.Write(0x01);

    programmableInterruptControllerMasterDataPort.Write(0x00);
    programmableInterruptControllerSlaveDataPort.Write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.size  = 256 * sizeof(GateDescriptor) - 1;
    idt.base  = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager()
{
    Deactivate();
}

uint16_t InterruptManager::HardwareInterruptOffset()
{
    return hardwareInterruptOffset;
}

void InterruptManager::Activate()
{
    asm("sti");
}

void InterruptManager::Deactivate()
{
    asm("cli");
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
    if(hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset+16)
    {
        if(interrupt == hardwareInterruptOffset + 1)
        {
            HandleKeyboardInterrupt();
        }
        
        programmableInterruptControllerMasterCommandPort.Write(0x20);
        if(hardwareInterruptOffset + 8 <= interrupt)
            programmableInterruptControllerSlaveCommandPort.Write(0x20);
    }
    
    return esp;
}

void InterruptManager::HandleKeyboardInterrupt()
{
    Port8Bit dataport(0x60);
    uint8_t key = dataport.Read();
    
    static char* keyboardmap =
        "  1234567890-=  qwertyuiop[]  asdfghjkl;'` \\zxcvbnm,./   ";
    
    if(key < 58)
    {
        ProcessKeyboardInput(keyboardmap[key]);
    }
}
