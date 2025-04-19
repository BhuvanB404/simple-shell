MEMORY
{
    flash (rx) : ORIGIN = 0x80000000, LENGTH = 32K
    ram (rwx) : ORIGIN =  0x20000000, LENGTH = 6K
}

_eram = ORIGIN(ram) + LENGTH(ram);
_stack = _eram;  /*stack begins at end of ram, growing downwardss .*/

SECTIONS
{
    .text : ALIGN(4)
{
    KEEP(*(.ivt)) /*vector interrupt rable */
    *(.text*)
    . = ALIGN(4);
    _etext = .;
    } >flash
}