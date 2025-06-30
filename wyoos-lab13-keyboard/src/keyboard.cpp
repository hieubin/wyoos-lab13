#include "types.h"

void printf(const char* str);
void printNum(int32_t num);
void SolveQuadraticEquation();

static char input_buffer[32];
static int input_pos = 0;
static bool input_ready = false;

static int coefficient_stage = 0;
static int32_t a = 0, b = 0, c = 0;

void ProcessKeyboardInput(char key)
{
    if(key == 'q' || key == 'Q')
    {
        printf("\nShutting down...\n");
        asm("cli");
        asm("hlt");
        while(1);
    }
    else if(key == '\n' || key == '\r')
    {
        input_buffer[input_pos] = '\0';
        input_ready = true;
        printf("\n");
        
        int32_t number = 0;
        bool negative = false;
        int start = 0;
        
        if(input_buffer[0] == '-')
        {
            negative = true;
            start = 1;
        }
        
        for(int i = start; i < input_pos; i++)
        {
            if(input_buffer[i] >= '0' && input_buffer[i] <= '9')
            {
                number = number * 10 + (input_buffer[i] - '0');
            }
        }
        
        if(negative) number = -number;
        
        switch(coefficient_stage)
        {
            case 0:
                a = number;
                coefficient_stage = 1;
                printf("Enter coefficient b: ");
                break;
            case 1:
                b = number;
                coefficient_stage = 2;
                printf("Enter coefficient c: ");
                break;
            case 2:
                c = number;
                coefficient_stage = 3;
                SolveQuadraticEquation();
                break;
        }
        
        input_pos = 0;
        input_ready = false;
    }
    else if(key == '\b')
    {
        if(input_pos > 0)
        {
            input_pos--;
            printf("\b \b");
        }
    }
    else if(key >= ' ' && key <= '~' && input_pos < 31)
    {
        input_buffer[input_pos] = key;
        input_pos++;
        
        char temp[2];
        temp[0] = key;
        temp[1] = '\0';
        printf(temp);
    }
}

void SolveQuadraticEquation()
{
    printf("\nSolving equation: ");
    printNum(a);
    printf("x^2 + ");
    printNum(b);
    printf("x + ");
    printNum(c);
    printf(" = 0\n");
    
    if(a == 0)
    {
        printf("Not a quadratic equation (a = 0)\n");
        if(b != 0)
        {
            printf("Linear solution: x = ");
            printNum(-c / b);
            printf("\n");
        }
        else
        {
            printf("No solution\n");
        }
        return;
    }
    
    int32_t discriminant = b * b - 4 * a * c;
    
    printf("Discriminant = ");
    printNum(discriminant);
    printf("\n");
    
    if(discriminant > 0)
    {
        printf("Two real solutions exist\n");
        
        int32_t sqrt_disc = 0;
        for(int32_t i = 1; i * i <= discriminant; i++)
        {
            if(i * i == discriminant)
            {
                sqrt_disc = i;
                break;
            }
        }
        
        if(sqrt_disc > 0)
        {
            int32_t x1 = (-b + sqrt_disc) / (2 * a);
            int32_t x2 = (-b - sqrt_disc) / (2 * a);
            
            printf("x1 = ");
            printNum(x1);
            printf("\n");
            printf("x2 = ");
            printNum(x2);
            printf("\n");
        }
        else
        {
            printf("Solutions involve square roots\n");
            printf("x1 = (-");
            printNum(b);
            printf(" + sqrt(");
            printNum(discriminant);
            printf(")) / ");
            printNum(2 * a);
            printf("\n");
            printf("x2 = (-");
            printNum(b);
            printf(" - sqrt(");
            printNum(discriminant);
            printf(")) / ");
            printNum(2 * a);
            printf("\n");
        }
    }
    else if(discriminant == 0)
    {
        printf("One real solution exists\n");
        int32_t x = -b / (2 * a);
        printf("x = ");
        printNum(x);
        printf("\n");
    }
    else
    {
        printf("No real solutions (complex solutions)\n");
    }
    
    printf("\nPress any key to solve another equation...\n");
    coefficient_stage = 0;
    a = b = c = 0;
}

void StartQuadraticSolver()
{
    printf("QUADRATIC EQUATION SOLVER\n");
    printf("ax^2 + bx + c = 0\n");
    printf("Press 'q' to quit\n\n");
    printf("Enter coefficient a: ");
    coefficient_stage = 0;
}
