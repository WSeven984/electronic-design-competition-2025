/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2' in SOPC Builder design 'mysystem'
 * SOPC Builder design path: ../../mysystem.sopcinfo
 *
 * Generated: Sat Aug 02 19:03:55 CST 2025
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x01001820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 100000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x19
#define ALT_CPU_DCACHE_BYPASS_MASK 0x80000000
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x00000020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 100000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_EXTRA_EXCEPTION_INFO
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 4096
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x19
#define ALT_CPU_NAME "nios2"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x01001000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x01001820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 100000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x19
#define NIOS2_DCACHE_BYPASS_MASK 0x80000000
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x00000020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_EXTRA_EXCEPTION_INFO
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x19
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x01001000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_EPCS_FLASH_CONTROLLER
#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_UART
#define __ALTERA_NIOS2_GEN2
#define __ALTPLL
#define __IR_HT6221_DECODE_IRQ
#define __LCD_CONTROL


/*
 * Ir_HT6221_Decode_Irq_0 configuration
 *
 */

#define ALT_MODULE_CLASS_Ir_HT6221_Decode_Irq_0 Ir_HT6221_Decode_Irq
#define IR_HT6221_DECODE_IRQ_0_BASE 0x10000b0
#define IR_HT6221_DECODE_IRQ_0_IRQ 3
#define IR_HT6221_DECODE_IRQ_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define IR_HT6221_DECODE_IRQ_0_NAME "/dev/Ir_HT6221_Decode_Irq_0"
#define IR_HT6221_DECODE_IRQ_0_SPAN 8
#define IR_HT6221_DECODE_IRQ_0_TYPE "Ir_HT6221_Decode_Irq"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone IV E"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_BASE 0x1002050
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_BASE 0x1002050
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_BASE 0x1002050
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "mysystem"


/*
 * altpll_0 configuration
 *
 */

#define ALTPLL_0_BASE 0x1002020
#define ALTPLL_0_IRQ -1
#define ALTPLL_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ALTPLL_0_NAME "/dev/altpll_0"
#define ALTPLL_0_SPAN 16
#define ALTPLL_0_TYPE "altpll"
#define ALT_MODULE_CLASS_altpll_0 altpll


/*
 * epcs configuration
 *
 */

#define ALT_MODULE_CLASS_epcs altera_avalon_epcs_flash_controller
#define EPCS_BASE 0x1001000
#define EPCS_IRQ 2
#define EPCS_IRQ_INTERRUPT_CONTROLLER_ID 0
#define EPCS_NAME "/dev/epcs"
#define EPCS_REGISTER_OFFSET 1024
#define EPCS_SPAN 2048
#define EPCS_TYPE "altera_avalon_epcs_flash_controller"


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 32
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK none


/*
 * jtag_uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart
#define JTAG_UART_0_BASE 0x1002050
#define JTAG_UART_0_IRQ 0
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8


/*
 * lcd configuration
 *
 */

#define ALT_MODULE_CLASS_lcd lcd_control
#define LCD_BASE 0x1002058
#define LCD_IRQ -1
#define LCD_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LCD_NAME "/dev/lcd"
#define LCD_SPAN 4
#define LCD_TYPE "lcd_control"


/*
 * lcd_bl configuration
 *
 */

#define ALT_MODULE_CLASS_lcd_bl altera_avalon_pio
#define LCD_BL_BASE 0x1002040
#define LCD_BL_BIT_CLEARING_EDGE_REGISTER 0
#define LCD_BL_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LCD_BL_CAPTURE 0
#define LCD_BL_DATA_WIDTH 1
#define LCD_BL_DO_TEST_BENCH_WIRING 0
#define LCD_BL_DRIVEN_SIM_VALUE 0
#define LCD_BL_EDGE_TYPE "NONE"
#define LCD_BL_FREQ 100000000
#define LCD_BL_HAS_IN 0
#define LCD_BL_HAS_OUT 1
#define LCD_BL_HAS_TRI 0
#define LCD_BL_IRQ -1
#define LCD_BL_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LCD_BL_IRQ_TYPE "NONE"
#define LCD_BL_NAME "/dev/lcd_bl"
#define LCD_BL_RESET_VALUE 1
#define LCD_BL_SPAN 16
#define LCD_BL_TYPE "altera_avalon_pio"


/*
 * lcd_rst configuration
 *
 */

#define ALT_MODULE_CLASS_lcd_rst altera_avalon_pio
#define LCD_RST_BASE 0x1002030
#define LCD_RST_BIT_CLEARING_EDGE_REGISTER 0
#define LCD_RST_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LCD_RST_CAPTURE 0
#define LCD_RST_DATA_WIDTH 1
#define LCD_RST_DO_TEST_BENCH_WIRING 0
#define LCD_RST_DRIVEN_SIM_VALUE 0
#define LCD_RST_EDGE_TYPE "NONE"
#define LCD_RST_FREQ 100000000
#define LCD_RST_HAS_IN 0
#define LCD_RST_HAS_OUT 1
#define LCD_RST_HAS_TRI 0
#define LCD_RST_IRQ -1
#define LCD_RST_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LCD_RST_IRQ_TYPE "NONE"
#define LCD_RST_NAME "/dev/lcd_rst"
#define LCD_RST_RESET_VALUE 1
#define LCD_RST_SPAN 16
#define LCD_RST_TYPE "altera_avalon_pio"


/*
 * pio0_inc configuration
 *
 */

#define ALT_MODULE_CLASS_pio0_inc altera_avalon_pio
#define PIO0_INC_BASE 0x10000a0
#define PIO0_INC_BIT_CLEARING_EDGE_REGISTER 0
#define PIO0_INC_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO0_INC_CAPTURE 0
#define PIO0_INC_DATA_WIDTH 32
#define PIO0_INC_DO_TEST_BENCH_WIRING 0
#define PIO0_INC_DRIVEN_SIM_VALUE 0
#define PIO0_INC_EDGE_TYPE "NONE"
#define PIO0_INC_FREQ 100000000
#define PIO0_INC_HAS_IN 0
#define PIO0_INC_HAS_OUT 1
#define PIO0_INC_HAS_TRI 0
#define PIO0_INC_IRQ -1
#define PIO0_INC_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO0_INC_IRQ_TYPE "NONE"
#define PIO0_INC_NAME "/dev/pio0_inc"
#define PIO0_INC_RESET_VALUE 214748
#define PIO0_INC_SPAN 16
#define PIO0_INC_TYPE "altera_avalon_pio"


/*
 * pio10_filter_write configuration
 *
 */

#define ALT_MODULE_CLASS_pio10_filter_write altera_avalon_pio
#define PIO10_FILTER_WRITE_BASE 0x1000000
#define PIO10_FILTER_WRITE_BIT_CLEARING_EDGE_REGISTER 0
#define PIO10_FILTER_WRITE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO10_FILTER_WRITE_CAPTURE 0
#define PIO10_FILTER_WRITE_DATA_WIDTH 1
#define PIO10_FILTER_WRITE_DO_TEST_BENCH_WIRING 0
#define PIO10_FILTER_WRITE_DRIVEN_SIM_VALUE 0
#define PIO10_FILTER_WRITE_EDGE_TYPE "NONE"
#define PIO10_FILTER_WRITE_FREQ 100000000
#define PIO10_FILTER_WRITE_HAS_IN 0
#define PIO10_FILTER_WRITE_HAS_OUT 1
#define PIO10_FILTER_WRITE_HAS_TRI 0
#define PIO10_FILTER_WRITE_IRQ -1
#define PIO10_FILTER_WRITE_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO10_FILTER_WRITE_IRQ_TYPE "NONE"
#define PIO10_FILTER_WRITE_NAME "/dev/pio10_filter_write"
#define PIO10_FILTER_WRITE_RESET_VALUE 0
#define PIO10_FILTER_WRITE_SPAN 16
#define PIO10_FILTER_WRITE_TYPE "altera_avalon_pio"


/*
 * pio1_sw configuration
 *
 */

#define ALT_MODULE_CLASS_pio1_sw altera_avalon_pio
#define PIO1_SW_BASE 0x1000090
#define PIO1_SW_BIT_CLEARING_EDGE_REGISTER 0
#define PIO1_SW_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO1_SW_CAPTURE 0
#define PIO1_SW_DATA_WIDTH 8
#define PIO1_SW_DO_TEST_BENCH_WIRING 0
#define PIO1_SW_DRIVEN_SIM_VALUE 0
#define PIO1_SW_EDGE_TYPE "NONE"
#define PIO1_SW_FREQ 100000000
#define PIO1_SW_HAS_IN 1
#define PIO1_SW_HAS_OUT 0
#define PIO1_SW_HAS_TRI 0
#define PIO1_SW_IRQ -1
#define PIO1_SW_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO1_SW_IRQ_TYPE "NONE"
#define PIO1_SW_NAME "/dev/pio1_sw"
#define PIO1_SW_RESET_VALUE 0
#define PIO1_SW_SPAN 16
#define PIO1_SW_TYPE "altera_avalon_pio"


/*
 * pio2_led configuration
 *
 */

#define ALT_MODULE_CLASS_pio2_led altera_avalon_pio
#define PIO2_LED_BASE 0x1000080
#define PIO2_LED_BIT_CLEARING_EDGE_REGISTER 0
#define PIO2_LED_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO2_LED_CAPTURE 0
#define PIO2_LED_DATA_WIDTH 8
#define PIO2_LED_DO_TEST_BENCH_WIRING 0
#define PIO2_LED_DRIVEN_SIM_VALUE 0
#define PIO2_LED_EDGE_TYPE "NONE"
#define PIO2_LED_FREQ 100000000
#define PIO2_LED_HAS_IN 0
#define PIO2_LED_HAS_OUT 1
#define PIO2_LED_HAS_TRI 0
#define PIO2_LED_IRQ -1
#define PIO2_LED_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO2_LED_IRQ_TYPE "NONE"
#define PIO2_LED_NAME "/dev/pio2_led"
#define PIO2_LED_RESET_VALUE 0
#define PIO2_LED_SPAN 16
#define PIO2_LED_TYPE "altera_avalon_pio"


/*
 * pio3_btn configuration
 *
 */

#define ALT_MODULE_CLASS_pio3_btn altera_avalon_pio
#define PIO3_BTN_BASE 0x1000070
#define PIO3_BTN_BIT_CLEARING_EDGE_REGISTER 0
#define PIO3_BTN_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO3_BTN_CAPTURE 0
#define PIO3_BTN_DATA_WIDTH 2
#define PIO3_BTN_DO_TEST_BENCH_WIRING 0
#define PIO3_BTN_DRIVEN_SIM_VALUE 0
#define PIO3_BTN_EDGE_TYPE "NONE"
#define PIO3_BTN_FREQ 100000000
#define PIO3_BTN_HAS_IN 1
#define PIO3_BTN_HAS_OUT 0
#define PIO3_BTN_HAS_TRI 0
#define PIO3_BTN_IRQ -1
#define PIO3_BTN_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO3_BTN_IRQ_TYPE "NONE"
#define PIO3_BTN_NAME "/dev/pio3_btn"
#define PIO3_BTN_RESET_VALUE 0
#define PIO3_BTN_SPAN 16
#define PIO3_BTN_TYPE "altera_avalon_pio"


/*
 * pio4_vpp_adc1 configuration
 *
 */

#define ALT_MODULE_CLASS_pio4_vpp_adc1 altera_avalon_pio
#define PIO4_VPP_ADC1_BASE 0x1000060
#define PIO4_VPP_ADC1_BIT_CLEARING_EDGE_REGISTER 0
#define PIO4_VPP_ADC1_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO4_VPP_ADC1_CAPTURE 0
#define PIO4_VPP_ADC1_DATA_WIDTH 8
#define PIO4_VPP_ADC1_DO_TEST_BENCH_WIRING 0
#define PIO4_VPP_ADC1_DRIVEN_SIM_VALUE 0
#define PIO4_VPP_ADC1_EDGE_TYPE "NONE"
#define PIO4_VPP_ADC1_FREQ 100000000
#define PIO4_VPP_ADC1_HAS_IN 1
#define PIO4_VPP_ADC1_HAS_OUT 0
#define PIO4_VPP_ADC1_HAS_TRI 0
#define PIO4_VPP_ADC1_IRQ -1
#define PIO4_VPP_ADC1_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO4_VPP_ADC1_IRQ_TYPE "NONE"
#define PIO4_VPP_ADC1_NAME "/dev/pio4_vpp_adc1"
#define PIO4_VPP_ADC1_RESET_VALUE 0
#define PIO4_VPP_ADC1_SPAN 16
#define PIO4_VPP_ADC1_TYPE "altera_avalon_pio"


/*
 * pio5_vpp_adc0 configuration
 *
 */

#define ALT_MODULE_CLASS_pio5_vpp_adc0 altera_avalon_pio
#define PIO5_VPP_ADC0_BASE 0x1000050
#define PIO5_VPP_ADC0_BIT_CLEARING_EDGE_REGISTER 0
#define PIO5_VPP_ADC0_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO5_VPP_ADC0_CAPTURE 0
#define PIO5_VPP_ADC0_DATA_WIDTH 8
#define PIO5_VPP_ADC0_DO_TEST_BENCH_WIRING 0
#define PIO5_VPP_ADC0_DRIVEN_SIM_VALUE 0
#define PIO5_VPP_ADC0_EDGE_TYPE "NONE"
#define PIO5_VPP_ADC0_FREQ 100000000
#define PIO5_VPP_ADC0_HAS_IN 1
#define PIO5_VPP_ADC0_HAS_OUT 0
#define PIO5_VPP_ADC0_HAS_TRI 0
#define PIO5_VPP_ADC0_IRQ -1
#define PIO5_VPP_ADC0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO5_VPP_ADC0_IRQ_TYPE "NONE"
#define PIO5_VPP_ADC0_NAME "/dev/pio5_vpp_adc0"
#define PIO5_VPP_ADC0_RESET_VALUE 0
#define PIO5_VPP_ADC0_SPAN 16
#define PIO5_VPP_ADC0_TYPE "altera_avalon_pio"


/*
 * pio6_mul configuration
 *
 */

#define ALT_MODULE_CLASS_pio6_mul altera_avalon_pio
#define PIO6_MUL_BASE 0x1000040
#define PIO6_MUL_BIT_CLEARING_EDGE_REGISTER 0
#define PIO6_MUL_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO6_MUL_CAPTURE 0
#define PIO6_MUL_DATA_WIDTH 8
#define PIO6_MUL_DO_TEST_BENCH_WIRING 0
#define PIO6_MUL_DRIVEN_SIM_VALUE 0
#define PIO6_MUL_EDGE_TYPE "NONE"
#define PIO6_MUL_FREQ 100000000
#define PIO6_MUL_HAS_IN 0
#define PIO6_MUL_HAS_OUT 1
#define PIO6_MUL_HAS_TRI 0
#define PIO6_MUL_IRQ -1
#define PIO6_MUL_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO6_MUL_IRQ_TYPE "NONE"
#define PIO6_MUL_NAME "/dev/pio6_mul"
#define PIO6_MUL_RESET_VALUE 254
#define PIO6_MUL_SPAN 16
#define PIO6_MUL_TYPE "altera_avalon_pio"


/*
 * pio7_filter_work configuration
 *
 */

#define ALT_MODULE_CLASS_pio7_filter_work altera_avalon_pio
#define PIO7_FILTER_WORK_BASE 0x1000030
#define PIO7_FILTER_WORK_BIT_CLEARING_EDGE_REGISTER 0
#define PIO7_FILTER_WORK_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO7_FILTER_WORK_CAPTURE 0
#define PIO7_FILTER_WORK_DATA_WIDTH 1
#define PIO7_FILTER_WORK_DO_TEST_BENCH_WIRING 0
#define PIO7_FILTER_WORK_DRIVEN_SIM_VALUE 0
#define PIO7_FILTER_WORK_EDGE_TYPE "NONE"
#define PIO7_FILTER_WORK_FREQ 100000000
#define PIO7_FILTER_WORK_HAS_IN 0
#define PIO7_FILTER_WORK_HAS_OUT 1
#define PIO7_FILTER_WORK_HAS_TRI 0
#define PIO7_FILTER_WORK_IRQ -1
#define PIO7_FILTER_WORK_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO7_FILTER_WORK_IRQ_TYPE "NONE"
#define PIO7_FILTER_WORK_NAME "/dev/pio7_filter_work"
#define PIO7_FILTER_WORK_RESET_VALUE 0
#define PIO7_FILTER_WORK_SPAN 16
#define PIO7_FILTER_WORK_TYPE "altera_avalon_pio"


/*
 * pio8_filter_addr configuration
 *
 */

#define ALT_MODULE_CLASS_pio8_filter_addr altera_avalon_pio
#define PIO8_FILTER_ADDR_BASE 0x1000020
#define PIO8_FILTER_ADDR_BIT_CLEARING_EDGE_REGISTER 0
#define PIO8_FILTER_ADDR_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO8_FILTER_ADDR_CAPTURE 0
#define PIO8_FILTER_ADDR_DATA_WIDTH 5
#define PIO8_FILTER_ADDR_DO_TEST_BENCH_WIRING 0
#define PIO8_FILTER_ADDR_DRIVEN_SIM_VALUE 0
#define PIO8_FILTER_ADDR_EDGE_TYPE "NONE"
#define PIO8_FILTER_ADDR_FREQ 100000000
#define PIO8_FILTER_ADDR_HAS_IN 0
#define PIO8_FILTER_ADDR_HAS_OUT 1
#define PIO8_FILTER_ADDR_HAS_TRI 0
#define PIO8_FILTER_ADDR_IRQ -1
#define PIO8_FILTER_ADDR_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO8_FILTER_ADDR_IRQ_TYPE "NONE"
#define PIO8_FILTER_ADDR_NAME "/dev/pio8_filter_addr"
#define PIO8_FILTER_ADDR_RESET_VALUE 0
#define PIO8_FILTER_ADDR_SPAN 16
#define PIO8_FILTER_ADDR_TYPE "altera_avalon_pio"


/*
 * pio9_filter_data configuration
 *
 */

#define ALT_MODULE_CLASS_pio9_filter_data altera_avalon_pio
#define PIO9_FILTER_DATA_BASE 0x1000010
#define PIO9_FILTER_DATA_BIT_CLEARING_EDGE_REGISTER 0
#define PIO9_FILTER_DATA_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO9_FILTER_DATA_CAPTURE 0
#define PIO9_FILTER_DATA_DATA_WIDTH 10
#define PIO9_FILTER_DATA_DO_TEST_BENCH_WIRING 0
#define PIO9_FILTER_DATA_DRIVEN_SIM_VALUE 0
#define PIO9_FILTER_DATA_EDGE_TYPE "NONE"
#define PIO9_FILTER_DATA_FREQ 100000000
#define PIO9_FILTER_DATA_HAS_IN 0
#define PIO9_FILTER_DATA_HAS_OUT 1
#define PIO9_FILTER_DATA_HAS_TRI 0
#define PIO9_FILTER_DATA_IRQ -1
#define PIO9_FILTER_DATA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO9_FILTER_DATA_IRQ_TYPE "NONE"
#define PIO9_FILTER_DATA_NAME "/dev/pio9_filter_data"
#define PIO9_FILTER_DATA_RESET_VALUE 0
#define PIO9_FILTER_DATA_SPAN 16
#define PIO9_FILTER_DATA_TYPE "altera_avalon_pio"


/*
 * sdram configuration
 *
 */

#define ALT_MODULE_CLASS_sdram altera_avalon_new_sdram_controller
#define SDRAM_BASE 0x0
#define SDRAM_CAS_LATENCY 3
#define SDRAM_CONTENTS_INFO
#define SDRAM_INIT_NOP_DELAY 0.0
#define SDRAM_INIT_REFRESH_COMMANDS 2
#define SDRAM_IRQ -1
#define SDRAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDRAM_IS_INITIALIZED 1
#define SDRAM_NAME "/dev/sdram"
#define SDRAM_POWERUP_DELAY 100.0
#define SDRAM_REFRESH_PERIOD 15.625
#define SDRAM_REGISTER_DATA_IN 1
#define SDRAM_SDRAM_ADDR_WIDTH 0x17
#define SDRAM_SDRAM_BANK_WIDTH 2
#define SDRAM_SDRAM_COL_WIDTH 9
#define SDRAM_SDRAM_DATA_WIDTH 16
#define SDRAM_SDRAM_NUM_BANKS 4
#define SDRAM_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_SDRAM_ROW_WIDTH 12
#define SDRAM_SHARED_DATA 0
#define SDRAM_SIM_MODEL_BASE 0
#define SDRAM_SPAN 16777216
#define SDRAM_STARVATION_INDICATOR 0
#define SDRAM_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_T_AC 5.5
#define SDRAM_T_MRD 3
#define SDRAM_T_RCD 20.0
#define SDRAM_T_RFC 70.0
#define SDRAM_T_RP 20.0
#define SDRAM_T_WR 14.0


/*
 * uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_uart_0 altera_avalon_uart
#define UART_0_BASE 0x1002000
#define UART_0_BAUD 115200
#define UART_0_DATA_BITS 8
#define UART_0_FIXED_BAUD 0
#define UART_0_FREQ 100000000
#define UART_0_IRQ 1
#define UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define UART_0_NAME "/dev/uart_0"
#define UART_0_PARITY 'N'
#define UART_0_SIM_CHAR_STREAM ""
#define UART_0_SIM_TRUE_BAUD 0
#define UART_0_SPAN 32
#define UART_0_STOP_BITS 1
#define UART_0_SYNC_REG_DEPTH 2
#define UART_0_TYPE "altera_avalon_uart"
#define UART_0_USE_CTS_RTS 0
#define UART_0_USE_EOP_REGISTER 0

#endif /* __SYSTEM_H_ */
