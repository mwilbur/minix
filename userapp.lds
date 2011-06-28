OUTPUT_ARCH("i386")
ENTRY(_start)
SECTIONS
{
	. = 0x0;
	_text = .;
	.text . : AT (0x0) {
		*(.init)
		*(.text)
		*(.text.*)
		. = ALIGN(4096);
	}
	_etext = .;
	etext = .;

	_data = .;
	.data . : {
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
		*(.data)
		*(.data.*)
	}
	_edata = .;
	_bss = .;
	.bss . : {
		*(.bss)
		*(.bss.*)
		*(COMMON)
	}
	_ebss = .;
	endbss = .;
	_end = .;
	end = .;

	/DISCARD/ :
	{
		*(.eh_frame)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
