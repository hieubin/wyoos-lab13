#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager
{
    friend class InterruptHandler;
    
protected:
    
    static InterruptManager* ActiveInterruptManager;

    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdtCodeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));
    
    static GateDescriptor interruptDescriptorTable[256];

    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));
    
    uint16_t hardwareInterruptOffset;
    static void SetInterruptDescriptorTableEntry(uint8_t interrupt,
        uint16_t codeSegmentSelectorOffset, void (*handler)(),
        uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType);

    Port8BitSlow programmableInterruptControllerMasterCommandPort;
    Port8BitSlow programmableInterruptControllerMasterDataPort;
    Port8BitSlow programmableInterruptControllerSlaveCommandPort;
    Port8BitSlow programmableInterruptControllerSlaveDataPort;

public:
    InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* globalDescriptorTable);
    ~InterruptManager();
    uint16_t HardwareInterruptOffset();
    void Activate();
    void Deactivate();
    
    static uint32_t HandleInterrupt(uint8_t interrupt, uint32_t esp);
    
    static void IgnoreInterruptRequest();
    static void HandleInterruptRequest0x00();
    static void HandleInterruptRequest0x01();
    
    void HandleKeyboardInterrupt();
};

void ProcessKeyboardInput(char c);
void SolveQuadraticEquation();

#endif
