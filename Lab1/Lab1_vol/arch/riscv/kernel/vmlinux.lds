BASE_ADDR = 0x80200000;
SECTIONS
{
	. = BASE_ADDR;
	.text : {
		*(.text.init)
		*(.text.entry)
		*(.text)
		*(.text.*)
	 }
	.rodata : { 
		*(.rodata) 
		*(.rodata.*) 
	}
	.data : { 
		*(.data) 
		*(.data.*)
	}
	bss_start = .;
	.bss : { 
		*(.sbss)
		*(.sbss.*)
		*(.bss)
		*(.bss.*)
	}
	bss_end = .;
	. += 0x8000;
	stack_top = .;
	_end = .;
}






