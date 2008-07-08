
	extern large start
	extern large _timer_handler_entry
	extern large _serial_in_handler1_entry
	extern large _serial_out_handler1_entry
	extern large _cpuexc_handler_entry
	extern large _unused_interrupt

f_vecter section code large align=4
	dl	_start	; 01  0xffff00
	dl	_unused_interrupt	; 02 0xffff04
	dl	_cpuexc_handler_entry	; 03 0xffff08
	dl	_unused_interrupt	; 04 0xffff0c
	dl	_unused_interrupt	; 05 0xffff10
	dl	_unused_interrupt	; 06 0xffff14
	dl	_unused_interrupt	; 07 0xffff18
	dl	_unused_interrupt	; 08 0xffff1c
	dl	_unused_interrupt	; 09 0xffff20
	dl	_unused_interrupt	; 10 0xffff24
	dl	_unused_interrupt	; 11 0xffff28
	dl	_unused_interrupt	; 12 0xffff2c
	dl	_unused_interrupt	; 13 0xffff30
	dl	_unused_interrupt	; 14 0xffff34
	dl	_unused_interrupt	; 15 0xffff38
	dl	_unused_interrupt	; 16 0xffff3c
	dl	_unused_interrupt	; 17 0xffff40
	dl	_unused_interrupt	; 18 0xffff44
	dl	_unused_interrupt	; 19 0xffff48
	dl	_timer_handler_entry	; 20 0xffff4c
	dl	_unused_interrupt	; 21 0xffff50
	dl	_unused_interrupt	; 22 0xffff54
	dl	_unused_interrupt	; 23 0xffff58
	dl	_unused_interrupt	; 24 0xffff5c
	dl	_unused_interrupt	; 25 0xffff60
	dl	_unused_interrupt	; 26 0xffff64
	dl	_unused_interrupt	; 27 0xffff68
	dl	_unused_interrupt	; 28 0xffff6c
	dl	_unused_interrupt	; 29 0xffff70
	dl	_unused_interrupt	; 30 0xffff74
	dl	_unused_interrupt	; 31 0xffff78
	dl	_unused_interrupt	; 32 0xffff7c
	dl	_unused_interrupt	; 33 0xffff80
	dl	_serial_in_handler1_entry	; 34 0xffff84
	dl	_serial_out_handler1_entry	; 35 0xffff88
	dl	_unused_interrupt	; 36 0xffff8c
	dl	_unused_interrupt	; 37 0xffff90
	dl	_unused_interrupt	; 38 0xffff94
	dl	_unused_interrupt	; 39 0xffff98
	dl	_unused_interrupt	; 40 0xffff9c
	dl	_unused_interrupt	; 41 0xffffa0
	dl	_unused_interrupt	; 42 0xffffa4
	dl	_unused_interrupt	; 43 0xffffa8
	dl	_unused_interrupt	; 44 0xffffac
	dl	_unused_interrupt	; 45 0xffffb0
	dl	_unused_interrupt	; 46 0xffffb4
	dl	_unused_interrupt	; 47 0xffffb8
	dl	_unused_interrupt	; 48 0xffffbc
	dl	_unused_interrupt	; 49 0xffffc0
	dl	_unused_interrupt	; 50 0xffffc4
	dl	_unused_interrupt	; 51 0xffffc8
	dl	_unused_interrupt	; 52 0xffffcc
	dl	_unused_interrupt	; 53 0xffffd0
	dl	_unused_interrupt	; 54 0xffffd4
	dl	_unused_interrupt	; 55 0xffffd8
	dl	_unused_interrupt	; 56 0xffffdc
	dl	_unused_interrupt	; 57 0xffffe0
	dl	_unused_interrupt	; 58 0xffffe4
	dl	_unused_interrupt	; 59 0xffffe8
	dl	_unused_interrupt	; 60 0xffffec
	dl	_unused_interrupt	; 61 0xfffff0
	dl	_unused_interrupt	; 62 0xfffff4
	dl	_unused_interrupt	; 63 0xfffff8
	dl	_unused_interrupt	; 64 0xfffffc
