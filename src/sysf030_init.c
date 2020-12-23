/* -------------------------------------------------------------
                         sysf030_init.c

   initialisiert MCU STM32F030F4P6 fuer einen "allgemeinen"
   Gebrauch.

   Das System wird auf 48MHz interner Takt eingestellt, die
   Funktion < sys_tick_handler > wird jede ms per Interrupt
   ausgefuehrt und in dieser wird eine Variable tick_ms
   hochgezaehlt.

   Der Takt fuer GPIO-Pins des Ports A und B wird einge-
   schaltet und im hier zugehoerigen Header sind mittels
   defines die GPIO - Pins als Input / Output konfigurierbar.

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    28.09.2016   R. seelig

   ------------------------------------------------------------- */

#include "sysf030_init.h"

volatile int tick_ms = 0;

void sys_tick_handler(void)
{
  tick_ms++;
}

void delay(int c)
{
  volatile int end_time = tick_ms + c;

  while (tick_ms < end_time)
  {
    __asm volatile("wfi");
  }
}

void systick_setup(void)
{
  systick_clear();
  systick_set_clocksource(STK_CSR_CLKSOURCE_EXT);
  systick_set_reload(6000);
  systick_interrupt_enable();
  systick_counter_enable();
}

void gpio_clkon(void)
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
}

void rcc_clock_setup_in_8mhzhse_out_48mhz()
// setzt Clock fuer externen 8MHz Quarz und einem Systemtakt
// von 48 MHz
{
  rcc_osc_on(RCC_HSE);
  rcc_wait_for_osc_ready(RCC_HSE);
  rcc_set_sysclk_source(RCC_HSE);

  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
  rcc_set_ppre(RCC_CFGR_PPRE_NODIV);

  flash_set_ws(FLASH_ACR_LATENCY_024_048MHZ);

  RCC_CFGR2 = RCC_CFGR2_PREDIV_DIV2;        // Taktvorteiler Quarz

  // (8MHz Quarz / 2)* 12 = 48MHz
  rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_MUL12);

  RCC_CFGR |= RCC_CFGR_PLLSRC;

  rcc_osc_on(RCC_PLL);
  rcc_wait_for_osc_ready(RCC_PLL);
  rcc_set_sysclk_source(RCC_PLL);

  rcc_apb1_frequency = 48000000;
  rcc_ahb_frequency = 48000000;
}

void sys_init(void)
{
  rcc_clock_setup_in_hsi_out_48mhz();
  systick_setup();
  gpio_clkon();
}

void sys_init_extclk(void)
{
  rcc_clock_setup_in_8mhzhse_out_48mhz();
  systick_setup();
  gpio_clkon();
}