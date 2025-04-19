extern int main(void);
extern unsigned int _stack;

void reset(void)
{
    main();

    // the while loop is for unexpected cases when we return from main
    while(1);
}

//for hardfault catches when interupt handling causes hardfaults.
void hardfault(void)
{
    while(1);
}

#define IVT_ARRAY_SIZE (48U)
typedef void(*isr_t)(void);
__attribute((used,section(".ivt")))
static const isr_t ivt[IVT_ARRAY_SIZE] = 
{
    (isr_t)&_stack,
    reset,
    0, //non mappable interrupt.
    hardfault,
};