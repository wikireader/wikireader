/*
 * interrupt vector handling
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(_VECTOR_H_)
#define _VECTOR_H_ 1

typedef void Vector_handler(void); //on actual definition:  __attribute__((interrupt_handler))

typedef enum {
//+MakeSystemCalls: vectors
	VECTOR_Reset = 0,
	VECTOR_reserved_1 = 1,
	VECTOR_ext_exception = 2,
	VECTOR_Undefined_instruction_exception = 3,
	VECTOR_reserved_4 = 4,
	VECTOR_reserved_5 = 5,
	VECTOR_Address_misaligned_exception = 6,
	VECTOR_NMI = 7,
	VECTOR_reserved_8 = 8,
	VECTOR_reserved_9 = 9,
	VECTOR_reserved_10 = 10,
	VECTOR_Illegal_interrupt_exception = 11,
	VECTOR_Software_exception_0 = 12,
	VECTOR_Software_exception_1 = 13,
	VECTOR_Software_exception_2 = 14,
	VECTOR_Software_exception_3 = 15,
	VECTOR_Port_input_interrupt_0 = 16,
	VECTOR_Port_input_interrupt_1 = 17,
	VECTOR_Port_input_interrupt_2 = 18,
	VECTOR_Port_input_interrupt_3 = 19,
	VECTOR_Key_input_interrupt_0 = 20,
	VECTOR_Key_input_interrupt_1 = 21,
	VECTOR_High_speed_DMA_Ch_0 = 22,
	VECTOR_High_speed_DMA_Ch_1 = 23,
	VECTOR_High_speed_DMA_Ch_2 = 24,
	VECTOR_High_speed_DMA_Ch_3 = 25,
	VECTOR_Intelligent_DMA = 26,
	VECTOR_reserved_27 = 27,
	VECTOR_reserved_28 = 28,
	VECTOR_reserved_29 = 29,
	VECTOR_16_bit_timer_0_compare_match_B = 30,
	VECTOR_16_bit_timer_0_compare_match_A = 31,
	VECTOR_reserved_32 = 32,
	VECTOR_reserved_33 = 33,
	VECTOR_16_bit_timer_1_compare_match_B = 34,
	VECTOR_16_bit_timer_1_compare_match_A = 35,
	VECTOR_reserved_36 = 36,
	VECTOR_reserved_37 = 37,
	VECTOR_16_bit_timer_2_compare_match_B = 38,
	VECTOR_16_bit_timer_2_compare_match_A = 39,
	VECTOR_reserved_40 = 40,
	VECTOR_reserved_41 = 41,
	VECTOR_16_bit_timer_3_compare_match_B = 42,
	VECTOR_16_bit_timer_3_compare_match_A = 43,
	VECTOR_reserved_44 = 44,
	VECTOR_reserved_45 = 45,
	VECTOR_16_bit_timer_4_compare_match_B = 46,
	VECTOR_16_bit_timer_4_compare_match_A = 47,
	VECTOR_reserved_48 = 48,
	VECTOR_reserved_49 = 49,
	VECTOR_16_bit_timer_5_compare_match_B = 50,
	VECTOR_16_bit_timer_5_compare_match_A = 51,
	VECTOR_reserved_52 = 52,
	VECTOR_reserved_53 = 53,
	VECTOR_reserved_54 = 54,
	VECTOR_reserved_55 = 55,
	VECTOR_Serial_interface_Ch_0_Receive_error = 56,
	VECTOR_Serial_interface_Ch_0_Receive_buffer_full = 57,
	VECTOR_Serial_interface_Ch_0_Transmit_buffer_empty = 58,
	VECTOR_reserved_59 = 59,
	VECTOR_Serial_interface_Ch_1_Receive_error = 60,
	VECTOR_Serial_interface_Ch_1_Receive_buffer_full = 61,
	VECTOR_Serial_interface_Ch_1_Transmit_buffer_empty = 62,
	VECTOR_A_D_converter_Result_out_of_range = 63,
	VECTOR_A_D_converter_End_of_conversion = 64,
	VECTOR_RTC = 65,
	VECTOR_reserved_66 = 66,
	VECTOR_reserved_67 = 67,
	VECTOR_Port_input_interrupt_4 = 68,
	VECTOR_Port_input_interrupt_5 = 69,
	VECTOR_Port_input_interrupt_6 = 70,
	VECTOR_Port_input_interrupt_7 = 71,
	VECTOR_reserved_72 = 72,
	VECTOR_LCDC = 73,
	VECTOR_reserved_74 = 74,
	VECTOR_reserved_75 = 75,
	VECTOR_Serial_interface_Ch_2_Receive_error = 76,
	VECTOR_Serial_interface_Ch_2_Receive_buffer_full = 77,
	VECTOR_Serial_interface_Ch_2_Transmit_buffer_empty = 78,
	VECTOR_reserved_79 = 79,
	VECTOR_reserved_80 = 80,
	VECTOR_SPI_Receive_DMA_request = 81,
	VECTOR_SPI_Transmit_DMA_request = 82,
	VECTOR_reserved_83 = 83,
	VECTOR_Port_input_interrupt_8_SPI_SPI_interrupt = 84,
	VECTOR_Port_input_interrupt_9_USB_PDREQ_USB_DMA_request = 85,
	VECTOR_Port_input_interrupt_10_USB_USBinterrupt = 86,
	VECTOR_Port_input_interrupt_11_DCSIO_DCSIO_interrupt = 87,
	VECTOR_Port_input_interrupt_12 = 88,
	VECTOR_Port_input_interrupt_13 = 89,
	VECTOR_Port_input_interrupt_14 = 90,
	VECTOR_Port_input_interrupt_15 = 91,
	VECTOR_reserved_92 = 92,
	VECTOR_reserved_93 = 93,
	VECTOR_I2S_interface_I2S_FIFO_empty = 94,
	VECTOR_reserved_95 = 95,
	VECTOR_reserved_96 = 96,
	VECTOR_reserved_97 = 97,
	VECTOR_reserved_98 = 98,
	VECTOR_reserved_99 = 99,
	VECTOR_reserved_100 = 100,
	VECTOR_reserved_101 = 101,
	VECTOR_reserved_102 = 102,
	VECTOR_reserved_103 = 103,
	VECTOR_reserved_104 = 104,
	VECTOR_reserved_105 = 105,
	VECTOR_reserved_106 = 106,
	VECTOR_reserved_107 = 107,
//-MakeSystemCalls: vectors
} Vector_type;


void Vector_initialise(void);

Vector_handler *Vector_get(Vector_type vector_number);

// for convenience, this returns previous vector
Vector_handler *Vector_set(Vector_type vector_number, Vector_handler *vector);


#endif
