#ifndef HWLIBS_H
#define HWLIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_usart.h>

#endif

#define USART_DBG               USART1
#define USART_DBG_HANDLER       USART1_IRQHandler

#ifdef __cplusplus
}
#endif

#endif // HWLIBS_H
