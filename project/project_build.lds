/* project_build.lds - Linker Script for Memory Configuration */

/* Define memory regions */
MEMORY
{
    ROM (rx)  : ORIGIN = 0x0000000000000000, LENGTH = 0x100000000
    RAM (rwx) : ORIGIN = 0x0000000000300000, LENGTH = 0x100000000
}

/* Define sections and their placement in memory */
SECTIONS
{
    /* Initial entry point */
    . = SEGMENT_START("text-segment", 0x0) + SIZEOF_HEADERS;
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x0));

    /* Interpreted section */
    .interp : { *(.interp) } > ROM :text

    /* Note sections */
    .note.gnu.property : { *(.note.gnu.property) } > ROM :text
    .note.gnu.build-id : { *(.note.gnu.build-id) } > ROM :text
    .note.ABI-tag : { *(.note.ABI-tag) } > ROM :text

    /* Hash sections */
    .hash : { *(.hash) } > ROM :text
    .gnu.hash : { *(.gnu.hash) } > ROM :text

    /* Dynamic symbol table */
    .dynsym : { *(.dynsym) } > ROM :text

    /* Dynamic string table */
    .dynstr : { *(.dynstr) } > ROM :text

    /* GNU version sections */
    .gnu.version : { *(.gnu.version) } > ROM :text
    .gnu.version_d : { *(.gnu.version_d) } > ROM :text
    .gnu.version_r : { *(.gnu.version_r) } > ROM :text

    /* Relocation sections */
    .rela.dyn : { *(.rela.dyn) } > ROM :text
    .rela.plt : { *(.rela.plt) } > ROM :text

    /* Initialization and finalization sections */
    .init : { *(.init) } > ROM :text
    .fini : { *(.fini) } > ROM :text

    /* Procedure linkage table */
    .plt : { *(.plt) } > ROM :text
    .plt.got : { *(.plt.got) } > ROM :text
    .plt.sec : { *(.plt.sec) } > ROM :text

    /* Code section */
    .text : { *(.text) } > ROM :text

    /* Read-only data section */
    .rodata : { *(.rodata) } > ROM :text

    /* Exception handling sections */
    .eh_frame_hdr : { *(.eh_frame_hdr) } > ROM :text
    .eh_frame : { *(.eh_frame) } > ROM :text

    /* Thread-local storage sections */
    .tdata : { *(.tdata) } > RAM :data
    .tbss : { *(.tbss) } > RAM :data

    /* Initialization and finalization arrays */
    .preinit_array : { *(.preinit_array) } > RAM :data
    .init_array : { *(.init_array) } > RAM :data
    .fini_array : { *(.fini_array) } > RAM :data

    /* Constructors and destructors */
    .ctors : { *(.ctors) } > RAM :data
    .dtors : { *(.dtors) } > RAM :data

    /* Data sections */
    .data.rel.ro : { *(.data.rel.ro) } > RAM :data
    .dynamic : { *(.dynamic) } > RAM :data
    .got : { *(.got) } > RAM :data
    .data : { *(.data) } > RAM :data
    .bss : { *(.bss) } > RAM :data

    /* Debugging sections */
    .stab : { *(.stab) } > RAM :data
    .stabstr : { *(.stabstr) } > RAM :data
    .stab.excl : { *(.stab.excl) } > RAM :data
    .stab.exclstr : { *(.stab.exclstr) } > RAM :data
    .stab.index : { *(.stab.index) } > RAM :data
    .stab.indexstr : { *(.stab.indexstr) } > RAM :data
    .comment : { *(.comment) } > RAM :data
    .gnu.build.attributes : { *(.gnu.build.attributes) } > RAM :data
    .debug : { *(.debug) } > RAM :data
    .line : { *(.line) } > RAM :data
    .debug_srcinfo : { *(.debug_srcinfo) } > RAM :data
    .debug_sfnames : { *(.debug_sfnames) } > RAM :data
    .debug_aranges : { *(.debug_aranges) } > RAM :data
    .debug_pubnames : { *(.debug_pubnames) } > RAM :data
    .debug_info : { *(.debug_info) } > RAM :data
    .debug_abbrev : { *(.debug_abbrev) } > RAM :data
    .debug_line : { *(.debug_line) } > RAM :data
    .debug_frame : { *(.debug_frame) } > RAM :data
    .debug_str : { *(.debug_str) } > RAM :data
    .debug_loc : { *(.debug_loc) } > RAM :data
    .debug_macinfo : { *(.debug_macinfo) } > RAM :data
    .debug_weaknames : { *(.debug_weaknames) } > RAM :data
    .debug_funcnames : { *(.debug_funcnames) } > RAM :data
    .debug_typenames : { *(.debug_typenames) } > RAM :data
    .debug_varnames : { *(.debug_varnames) } > RAM :data
    .debug_pubtypes : { *(.debug_pubtypes) } > RAM :data
    .debug_ranges : { *(.debug_ranges) } > RAM :data
    .debug_addr : { *(.debug_addr) } > RAM :data
    .debug_line_str : { *(.debug_line_str) } > RAM :data
    .debug_loclists : { *(.debug_loclists) } > RAM :data
    .debug_macro : { *(.debug_macro) } > RAM :data
    .debug_names : { *(.debug_names) } > RAM :data
    .debug_rnglists : { *(.debug_rnglists) } > RAM :data
    .debug_str_offsets : { *(.debug_str_offsets) } > RAM :data
    .debug_sup : { *(.debug_sup) } > RAM :data
    .gnu.attributes : { *(.gnu.attributes) } > RAM :data

    /* Discard sections */
    /DISCARD/ : { *(.gnu.lto_*) }
}

/* Define program headers */
PHDRS
{
    text PT_LOAD FILEHDR PHDRS;
    data PT_LOAD;
}