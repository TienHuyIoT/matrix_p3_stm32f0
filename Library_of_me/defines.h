/**
 *  Defines for your entire project at one place
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

#include "stm32f0xx.h"

//LED
//#define		led_0(val)		((val)? (GPIOA->BSRR = GPIO_PIN_2) : (GPIOA->BRR = GPIO_PIN_2))

#define		R0_on				(GPIOB->BSRR = GPIO_PIN_13)
#define		R0_off			(GPIOB->BRR = GPIO_PIN_13)
// #define		G0_on				(GPIOA->BSRR = GPIO_PIN_6)	//đã sử dụng cho E
// #define		G0_off			(GPIOA->BRR = GPIO_PIN_6)

#define		R1_on				(GPIOB->BSRR = GPIO_PIN_15)
#define		R1_off			(GPIOB->BRR = GPIO_PIN_15)
// #define		G1_on				(GPIOB->BSRR = GPIO_PIN_14)
// #define		G1_off			(GPIOB->BRR = GPIO_PIN_14)

#define		R2_on				(GPIOB->BSRR = GPIO_PIN_5)
#define		R2_off			(GPIOB->BRR = GPIO_PIN_5)
// #define		G2_on				(GPIOB->BSRR = GPIO_PIN_6)
// #define		G2_off			(GPIOB->BRR = GPIO_PIN_6)

#define		A(val)			((val)? (GPIOA->BSRR = GPIO_PIN_7) : (GPIOA->BRR = GPIO_PIN_7))
#define		B(val)			((val)? (GPIOA->BSRR = GPIO_PIN_3) : (GPIOA->BRR = GPIO_PIN_3))
#define		C(val)			((val)? (GPIOA->BSRR = GPIO_PIN_5) : (GPIOA->BRR = GPIO_PIN_5))
#define		D(val)			((val)? (GPIOB->BSRR = GPIO_PIN_4) : (GPIOB->BRR = GPIO_PIN_4))
#define		E(val)			((val)? (GPIOA->BSRR = GPIO_PIN_6) : (GPIOA->BRR = GPIO_PIN_6))

#define		CLK(val)		((val)? (GPIOB->BSRR = GPIO_PIN_3) : (GPIOB->BRR = GPIO_PIN_3))
#define		LAT(val)		((val)? (GPIOA->BSRR = GPIO_PIN_15) : (GPIOA->BRR = GPIO_PIN_15))
#define		OE(val)			((val)? (GPIOB->BSRR = GPIO_PIN_12) : (GPIOB->BRR = GPIO_PIN_12))


// #define		R0_on				(GPIOA->BSRR = GPIO_PIN_7)
// #define		R0_off			(GPIOA->BRR = GPIO_PIN_7)
// // #define		G0_on				(GPIOA->BSRR = GPIO_PIN_6)	//đã sử dụng cho E
// // #define		G0_off			(GPIOA->BRR = GPIO_PIN_6)

// #define		R1_on				(GPIOB->BSRR = GPIO_PIN_13)
// #define		R1_off			(GPIOB->BRR = GPIO_PIN_13)
// // #define		G1_on				(GPIOB->BSRR = GPIO_PIN_14)
// // #define		G1_off			(GPIOB->BRR = GPIO_PIN_14)

// #define		R2_on				(GPIOB->BSRR = GPIO_PIN_5)
// #define		R2_off			(GPIOB->BRR = GPIO_PIN_5)
// // #define		G2_on				(GPIOB->BSRR = GPIO_PIN_6)
// // #define		G2_off			(GPIOB->BRR = GPIO_PIN_6)

// #define		A(val)			((val)? (GPIOA->BSRR = GPIO_PIN_15) : (GPIOA->BRR = GPIO_PIN_15))
// #define		B(val)			((val)? (GPIOB->BSRR = GPIO_PIN_3) : (GPIOB->BRR = GPIO_PIN_3))
// #define		C(val)			((val)? (GPIOB->BSRR = GPIO_PIN_4) : (GPIOB->BRR = GPIO_PIN_4))
// #define		D(val)			((val)? (GPIOB->BSRR = GPIO_PIN_15) : (GPIOB->BRR = GPIO_PIN_15))
// #define		E(val)			((val)? (GPIOA->BSRR = GPIO_PIN_6) : (GPIOA->BRR = GPIO_PIN_6))

// #define		CLK(val)		((val)? (GPIOA->BSRR = GPIO_PIN_5) : (GPIOA->BRR = GPIO_PIN_5))
// #define		LAT(val)		((val)? (GPIOA->BSRR = GPIO_PIN_3) : (GPIOA->BRR = GPIO_PIN_3))
// #define		OE(val)			((val)? (GPIOB->BSRR = GPIO_PIN_12) : (GPIOB->BRR = GPIO_PIN_12))


#endif
