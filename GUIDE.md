# Quadratic Equation Solver OS - Student Guide

## What is this project?

This is a **simple operating system** that can solve quadratic equations! It's like a calculator, but it runs directly on the computer without any other operating system (like Windows or Linux) underneath it.

## The Big Picture - What happens when you boot this OS?

Think of it like this sequence:
1. **Computer starts up** → Boot loader runs
2. **Boot loader** → Loads and starts our kernel
3. **Kernel** → Sets up the computer and starts the equation solver
4. **User** → Types in numbers and gets solutions!

## How the Code is Organized

### 📁 File Structure
```
wyoos-lab13-keyboard/
├── src/
│   ├── loader.s          # Boot loader (Assembly)
│   ├── kernel.cpp        # Main kernel code
│   ├── gdt.cpp          # Memory management setup
│   ├── interrupts.cpp   # Handles keyboard input
│   ├── interrupts.s     # Assembly interrupt handlers
│   ├── keyboard.cpp     # Processes what you type
│   └── port.cpp         # Talks to hardware
├── Makefile             # Build instructions
└── README.md           # Project info
```

## The Journey: From Boot to Equation Solving

### 🚀 Step 1: Boot Process (`loader.s:15-25`)
**What it does:** This is the very first code that runs when you boot the OS.

```assembly
loader:
    mov $kernel_stack, %esp    # Set up stack memory
    call callConstructors     # Initialize C++ objects
    call kernelMain          # Jump to main kernel
```

**Why it's needed:** Computers need to be told how to start. This code prepares the computer and then calls our main kernel function.

### 🧠 Step 2: Kernel Initialization (`kernel.cpp:94-107`)
**What it does:** Sets up the operating system basics.

```cpp
extern "C" void kernelMain(void* multibootStruct, uint32_t magicNumber)
{
    GlobalDescriptorTable gdt;           // Set up memory management
    InterruptManager interrupts(0x20, &gdt);  // Set up keyboard handling
    
    printf("Initializing system...\n"); // Tell user we're starting
    interrupts.Activate();              // Turn on keyboard input
    StartQuadraticSolver();             // Start the main program
    
    while(1);  // Keep running forever
}
```

**Why it's needed:** Before we can use the keyboard or display text, we need to tell the computer how to manage memory and handle input.

### 🎯 Step 3: Memory Management (`gdt.cpp:3-13`)
**What it does:** Creates a "map" of computer memory so programs know where they can store data.

```cpp
GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0),
      codeSegmentSelector(0, 64*1024*1024, 0x9A),    // Code can go here
      dataSegmentSelector(0, 64*1024*1024, 0x92)     // Data can go here
{
    // Tell the CPU about our memory layout
    asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
}
```

**Why it's needed:** Without this, the computer wouldn't know where to put our program code and data safely.

### ⌨️ Step 4: Keyboard Setup (`interrupts.cpp:20-58`)
**What it does:** Sets up the computer to "listen" for keyboard presses.

**The flow:**
1. **Set up interrupt table** - Create a list of what to do when keys are pressed
2. **Configure hardware** - Tell the keyboard controller how to work
3. **Activate interrupts** - Start listening for key presses

**Why it's needed:** Without this, the computer wouldn't know when you press keys!

### 🎹 Step 5: Handling Key Presses (`interrupts.cpp:97-123`)
**What it does:** When you press a key, this code figures out which key it was.

```cpp
void InterruptManager::HandleKeyboardInterrupt()
{
    Port8Bit dataport(0x60);        // Read from keyboard hardware
    uint8_t key = dataport.Read();   // Get the key code
    
    // Convert key code to actual character
    static char keyboardmap[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
        // ... more keys
    };
    
    if(key < 128 && keyboardmap[key] != 0) {
        ProcessKeyboardInput(keyboardmap[key]);  // Send to equation solver
    }
}
```

**Why it's needed:** The keyboard sends numbers (key codes), but we need actual letters and numbers for our program.

### 🔢 Step 6: Processing Input (`keyboard.cpp:13-81`)
**What it does:** Takes the keys you press and builds numbers for the equation solver.

**The flow:**
1. **Collect characters** - Build up numbers as you type
2. **Handle Enter** - When you press Enter, convert text to number
3. **Store coefficients** - Save a, b, c values for the equation
4. **Handle backspace** - Let you fix mistakes

```cpp
void ProcessKeyboardInput(char key)
{
    if(key == '\n' || key == '\r') {  // Enter pressed
        // Convert text to number
        int32_t number = 0;
        for(int i = start; i < input_pos; i++) {
            number = number * 10 + (input_buffer[i] - '0');
        }
        
        // Store in a, b, or c depending on stage
        switch(coefficient_stage) {
            case 0: a = number; break;
            case 1: b = number; break; 
            case 2: c = number; SolveQuadraticEquation(); break;
        }
    }
}
```

### 🧮 Step 7: Solving the Equation (`keyboard.cpp:83-176`)
**What it does:** Uses the quadratic formula to find solutions.

**The math:**
- For equation ax² + bx + c = 0
- Discriminant = b² - 4ac
- If discriminant > 0: Two solutions
- If discriminant = 0: One solution  
- If discriminant < 0: No real solutions

```cpp
void SolveQuadraticEquation()
{
    int32_t discriminant = b * b - 4 * a * c;
    
    if(discriminant > 0) {
        printf("Two real solutions exist\n");
        // Calculate x1 and x2
    }
    else if(discriminant == 0) {
        printf("One real solution exists\n");
        // Calculate single solution
    }
    else {
        printf("No real solutions\n");
    }
}
```

### 📺 Step 8: Display Output (`kernel.cpp:5-48`)
**What it does:** Shows text on screen by writing directly to video memory.

```cpp
void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;  // Screen memory
    
    for(int i = 0; str[i] != '\0'; ++i) {
        VideoMemory[80*y+x] = str[i];  // Put character on screen
        x++;  // Move to next position
    }
}
```

**Why it's needed:** This is how we show the results to the user!

## Key Concepts for Your Presentation

### 1. **Bare Metal Programming**
- No Windows/Linux underneath
- We talk directly to hardware
- Much more control, but also more responsibility

### 2. **Interrupt-Driven Input**
- Computer "interrupts" our program when keys are pressed
- Like getting a phone call while doing homework
- We handle the "call" (key press) then continue

### 3. **Memory Management**
- We tell the computer where our code and data live
- Like organizing your desk before starting homework

### 4. **State Machine**
- Program remembers which coefficient (a, b, or c) we're entering
- Like following a recipe step by step

## Common Questions & Answers

**Q: Why assembly language?**
A: Some tasks (like booting) must be done in assembly because they're too low-level for C++.

**Q: What's an interrupt?**
A: It's like a doorbell - when pressed, it stops what you're doing to handle the visitor (keyboard input).

**Q: Why can't we just use printf from regular C++?**
A: We don't have an operating system to provide printf, so we write our own!

**Q: How does the computer know to run our code?**
A: The bootloader follows a standard protocol (multiboot) that tells it where our kernel starts.

## Build Process

1. **Compile** C++ files → Object files (.o)
2. **Assemble** Assembly files → Object files (.o)  
3. **Link** All objects → Binary executable
4. **Create ISO** → Bootable disc image
5. **Boot** in virtual machine

## Summary

This OS demonstrates fundamental operating system concepts:
- **Boot process** - How computers start
- **Hardware interaction** - Talking to keyboard and screen
- **Memory management** - Organizing computer memory
- **Interrupt handling** - Responding to external events
- **User interaction** - Getting input and showing output

The code flows from boot → kernel setup → interrupt handling → user input → equation solving → display results, creating a complete interactive system!