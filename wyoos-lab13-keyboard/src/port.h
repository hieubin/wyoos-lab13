#ifndef _PORT_H_
#define _PORT_H_

#include "types.h"

class Port
{
protected:
    Port(uint16_t portnum);
    ~Port();
    uint16_t portnumber;
};

class Port8Bit : public Port
{
public:
    Port8Bit(uint16_t portnum);
    ~Port8Bit();
    
    virtual void Write(uint8_t data);
    virtual uint8_t Read();
};

class Port8BitSlow : public Port8Bit
{
public:
    Port8BitSlow(uint16_t portnum);
    ~Port8BitSlow();
    void Write(uint8_t data);
};

#endif
