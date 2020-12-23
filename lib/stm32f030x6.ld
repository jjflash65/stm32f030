MEMORY
{
    ram : ORIGIN = 0x20000000, LENGTH = 4K
    rom : ORIGIN = 0x08000000, LENGTH = 32K
}

INCLUDE libopencm3_stm32f0.ld
