ENTRY(start)

SECTIONS {
    . = 0x10000000;
    __user_base = .;
    
    .text :{
        KEEP(*(.text.start));
        *(.text .text.*);
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {  
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        /* __bss_end = .; */
        
        . = ALIGN(16);
        . += 128 * 1024; /* 128KB */
        __stack_top = .;
        ASSERT(. < 0x1FFE0000, "too large executable");
    }

}

