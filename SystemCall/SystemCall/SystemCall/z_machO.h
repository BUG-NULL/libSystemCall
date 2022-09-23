//
//  z_machO.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_machO_h
#define z_machO_h

#include <SystemCall/z_types.h>

/*
 * After MacOS X 10.1 when a new load command is added that is required to be
 * understood by the dynamic linker for the image to execute properly the
 * LC_REQ_DYLD bit will be or'ed into the load command constant.  If the dynamic
 * linker sees such a load command it it does not understand will issue a
 * "unknown load command required for execution" error and refuse to use the
 * image.  Other load commands without this bit that are not understood will
 * simply be ignored.
 */
#define     LC_REQ_DYLD 0x80000000

/* Constants for the cmd field of all load commands, the type */
#define     LC_SEGMENT    0x1    /* segment of this file to be mapped */
#define     LC_SYMTAB    0x2    /* link-edit stab symbol table info */
#define     LC_SYMSEG    0x3    /* link-edit gdb symbol table info (obsolete) */
#define     LC_THREAD    0x4    /* thread */
#define     LC_UNIXTHREAD    0x5    /* unix thread (includes a stack) */
#define     LC_LOADFVMLIB    0x6    /* load a specified fixed VM shared library */
#define     LC_IDFVMLIB    0x7    /* fixed VM shared library identification */
#define     LC_IDENT    0x8    /* object identification info (obsolete) */
#define     LC_FVMFILE    0x9    /* fixed VM file inclusion (internal use) */
#define     LC_PREPAGE      0xa     /* prepage command (internal use) */
#define     LC_DYSYMTAB    0xb    /* dynamic link-edit symbol table info */
#define     LC_LOAD_DYLIB    0xc    /* load a dynamically linked shared library */
#define     LC_ID_DYLIB    0xd    /* dynamically linked shared lib ident */
#define     LC_LOAD_DYLINKER 0xe    /* load a dynamic linker */
#define     LC_ID_DYLINKER    0xf    /* dynamic linker identification */
#define     LC_PREBOUND_DYLIB 0x10    /* modules prebound for a dynamically */
                /*  linked shared library */
#define     LC_ROUTINES    0x11    /* image routines */
#define     LC_SUB_FRAMEWORK 0x12    /* sub framework */
#define     LC_SUB_UMBRELLA 0x13    /* sub umbrella */
#define     LC_SUB_CLIENT    0x14    /* sub client */
#define     LC_SUB_LIBRARY  0x15    /* sub library */
#define     LC_TWOLEVEL_HINTS 0x16    /* two-level namespace lookup hints */
#define     LC_PREBIND_CKSUM  0x17    /* prebind checksum */

/*
 * load a dynamically linked shared library that is allowed to be missing
 * (all symbols are weak imported).
 */
#define     LC_LOAD_WEAK_DYLIB (0x18 | LC_REQ_DYLD)

#define     LC_SEGMENT_64    0x19    /* 64-bit segment of this file to be
                   mapped */
#define     LC_ROUTINES_64    0x1a    /* 64-bit image routines */
#define     LC_UUID        0x1b    /* the uuid */
#define     LC_RPATH       (0x1c | LC_REQ_DYLD)    /* runpath additions */
#define     LC_CODE_SIGNATURE 0x1d    /* local of code signature */
#define     LC_SEGMENT_SPLIT_INFO 0x1e /* local of info to split segments */
#define     LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD) /* load and re-export dylib */
#define     LC_LAZY_LOAD_DYLIB 0x20    /* delay load of dylib until first use */
#define     LC_ENCRYPTION_INFO 0x21    /* encrypted segment information */
#define     LC_DYLD_INFO     0x22    /* compressed dyld information */
#define     LC_DYLD_INFO_ONLY (0x22|LC_REQ_DYLD)    /* compressed dyld information only */
#define     LC_LOAD_UPWARD_DYLIB (0x23 | LC_REQ_DYLD) /* load upward dylib */
#define     LC_VERSION_MIN_MACOSX 0x24   /* build for MacOSX min OS version */
#define     LC_VERSION_MIN_IPHONEOS 0x25 /* build for iPhoneOS min OS version */
#define     LC_FUNCTION_STARTS 0x26 /* compressed table of function start addresses */
#define     LC_DYLD_ENVIRONMENT 0x27 /* string for dyld to treat
                    like environment variable */
#define     LC_MAIN (0x28|LC_REQ_DYLD) /* replacement for LC_UNIXTHREAD */
#define     LC_DATA_IN_CODE 0x29 /* table of non-instructions in __text */
#define     LC_SOURCE_VERSION 0x2A /* source version used to build binary */
#define     LC_DYLIB_CODE_SIGN_DRS 0x2B /* Code signing DRs copied from linked dylibs */
#define     LC_ENCRYPTION_INFO_64 0x2C /* 64-bit encrypted segment information */
#define     LC_LINKER_OPTION 0x2D /* linker options in MH_OBJECT files */
#define     LC_LINKER_OPTIMIZATION_HINT 0x2E /* optimization hints in MH_OBJECT files */
#define     LC_VERSION_MIN_TVOS 0x2F /* build for AppleTV min OS version */
#define     LC_VERSION_MIN_WATCHOS 0x30 /* build for Watch min OS version */
#define     LC_NOTE 0x31 /* arbitrary data included within a Mach-O file */
#define     LC_BUILD_VERSION 0x32 /* build for platform min OS version */
#define     LC_DYLD_EXPORTS_TRIE (0x33 | LC_REQ_DYLD) /* used with linkedit_data_command, payload is trie */
#define     LC_DYLD_CHAINED_FIXUPS (0x34 | LC_REQ_DYLD) /* used with linkedit_data_command */
#define     LC_FILESET_ENTRY (0x35 | LC_REQ_DYLD) /* used with fileset_entry_command */

/*
 * The flags field of a section structure is separated into two parts a section
 * type and section attributes.  The section types are mutually exclusive (it
 * can only have one type) but the section attributes are not (it may have more
 * than one attribute).
 */
#define     SECTION_TYPE         0x000000ff    /* 256 section types */
#define     SECTION_ATTRIBUTES     0xffffff00    /*  24 section attributes */

/* Constants for the type of a section */
#define     S_REGULAR        0x0    /* regular section */
#define     S_ZEROFILL        0x1    /* zero fill on demand section */
#define     S_CSTRING_LITERALS    0x2    /* section with only literal C strings*/
#define     S_4BYTE_LITERALS    0x3    /* section with only 4 byte literals */
#define     S_8BYTE_LITERALS    0x4    /* section with only 8 byte literals */
#define     S_LITERAL_POINTERS    0x5    /* section with only pointers to */
                    /*  literals */
/*
 * For the two types of symbol pointers sections and the symbol stubs section
 * they have indirect symbol table entries.  For each of the entries in the
 * section the indirect symbol table entries, in corresponding order in the
 * indirect symbol table, start at the index stored in the reserved1 field
 * of the section structure.  Since the indirect symbol table entries
 * correspond to the entries in the section the number of indirect symbol table
 * entries is inferred from the size of the section divided by the size of the
 * entries in the section.  For symbol pointers sections the size of the entries
 * in the section is 4 bytes and for symbol stubs sections the byte size of the
 * stubs is stored in the reserved2 field of the section structure.
 */
#define     S_NON_LAZY_SYMBOL_POINTERS    0x6    /* section with only non-lazy
                           symbol pointers */
#define     S_LAZY_SYMBOL_POINTERS        0x7    /* section with only lazy symbol
                           pointers */
#define     S_SYMBOL_STUBS            0x8    /* section with only symbol
                           stubs, byte size of stub in
                           the reserved2 field */
#define     S_MOD_INIT_FUNC_POINTERS    0x9    /* section with only function
                           pointers for initialization*/
#define     S_MOD_TERM_FUNC_POINTERS    0xa    /* section with only function
                           pointers for termination */
#define     S_COALESCED            0xb    /* section contains symbols that
                           are to be coalesced */
#define     S_GB_ZEROFILL            0xc    /* zero fill on demand section
                           (that can be larger than 4
                           gigabytes) */
#define     S_INTERPOSING            0xd    /* section with only pairs of
                           function pointers for
                           interposing */
#define     S_16BYTE_LITERALS        0xe    /* section with only 16 byte
                           literals */
#define     S_DTRACE_DOF            0xf    /* section contains
                           DTrace Object Format */
#define     S_LAZY_DYLIB_SYMBOL_POINTERS    0x10    /* section with only lazy
                           symbol pointers to lazy
                           loaded dylibs */
/*
 * Section types to support thread local variables
 */
#define     S_THREAD_LOCAL_REGULAR                   0x11  /* template of initial
                              values for TLVs */
#define     S_THREAD_LOCAL_ZEROFILL                  0x12  /* template of initial
                              values for TLVs */
#define     S_THREAD_LOCAL_VARIABLES                 0x13  /* TLV descriptors */
#define     S_THREAD_LOCAL_VARIABLE_POINTERS         0x14  /* pointers to TLV
                                                          descriptors */
#define     S_THREAD_LOCAL_INIT_FUNCTION_POINTERS    0x15  /* functions to call
                              to initialize TLV
                              values */
#define     S_INIT_FUNC_OFFSETS                      0x16  /* 32-bit offsets to
                              initializers */

/*
 * Constants for the section attributes part of the flags field of a section
 * structure.
 */
#define     SECTION_ATTRIBUTES_USR     0xff000000    /* User setable attributes */
#define     S_ATTR_PURE_INSTRUCTIONS 0x80000000    /* section contains only true
                           machine instructions */
#define     S_ATTR_NO_TOC          0x40000000    /* section contains coalesced
                           symbols that are not to be
                           in a ranlib table of
                           contents */
#define     S_ATTR_STRIP_STATIC_SYMS 0x20000000    /* ok to strip static symbols
                           in this section in files
                           with the MH_DYLDLINK flag */
#define     S_ATTR_NO_DEAD_STRIP     0x10000000    /* no dead stripping */
#define     S_ATTR_LIVE_SUPPORT     0x08000000    /* blocks are live if they
                           reference live blocks */
#define     S_ATTR_SELF_MODIFYING_CODE 0x04000000    /* Used with i386 code stubs
                           written on by dyld */
/*
 * If a segment contains any sections marked with S_ATTR_DEBUG then all
 * sections in that segment must have this attribute.  No section other than
 * a section marked with this attribute may reference the contents of this
 * section.  A section with this attribute may contain no symbols and must have
 * a section type S_REGULAR.  The static linker will not copy section contents
 * from sections with this attribute into its output file.  These sections
 * generally contain DWARF debugging info.
 */
#define     S_ATTR_DEBUG         0x02000000    /* a debug section */
#define     SECTION_ATTRIBUTES_SYS     0x00ffff00    /* system setable attributes */
#define     S_ATTR_SOME_INSTRUCTIONS 0x00000400    /* section contains some
                           machine instructions */
#define     S_ATTR_EXT_RELOC     0x00000200    /* section has external
                           relocation entries */
#define     S_ATTR_LOC_RELOC     0x00000100    /* section has local
                           relocation entries */

/*
 * The names of segments and sections in them are mostly meaningless to the
 * link-editor.  But there are few things to support traditional UNIX
 * executables that require the link-editor and assembler to use some names
 * agreed upon by convention.
 *
 * The initial protection of the "__TEXT" segment has write protection turned
 * off (not writeable).
 *
 * The link-editor will allocate common symbols at the end of the "__common"
 * section in the "__DATA" segment.  It will create the section and segment
 * if needed.
 */

/* The currently known segment names and the section names in those segments */

#define     SEG_PAGEZERO    "__PAGEZERO"    /* the pagezero segment which has no */
                    /* protections and catches NULL */
                    /* references for MH_EXECUTE files */


#define     SEG_TEXT    "__TEXT"    /* the tradition UNIX text segment */
#define     SECT_TEXT    "__text"    /* the real text part of the text */
                    /* section no headers, and no padding */
#define     SECT_FVMLIB_INIT0 "__fvmlib_init0"    /* the fvmlib initialization */
                        /*  section */
#define     SECT_FVMLIB_INIT1 "__fvmlib_init1"    /* the section following the */
                            /*  fvmlib initialization */
                        /*  section */

#define     SEG_DATA    "__DATA"    /* the tradition UNIX data segment */
#define     SECT_DATA    "__data"    /* the real initialized data section */
                    /* no padding, no bss overlap */
#define     SECT_BSS    "__bss"        /* the real uninitialized data section*/
                    /* no padding */
#define     SECT_COMMON    "__common"    /* the section common symbols are */
                    /* allocated in by the link editor */

#define     SEG_OBJC    "__OBJC"    /* objective-C runtime segment */
#define     SECT_OBJC_SYMBOLS "__symbol_table"    /* symbol table */
#define     SECT_OBJC_MODULES "__module_info"    /* module information */
#define     SECT_OBJC_STRINGS "__selector_strs"    /* string table */
#define     SECT_OBJC_REFS "__selector_refs"    /* string table */

#define     SEG_ICON     "__ICON"    /* the icon segment */
#define     SECT_ICON_HEADER "__header"    /* the icon headers */
#define     SECT_ICON_TIFF   "__tiff"    /* the icons in tiff format */

#define     SEG_LINKEDIT    "__LINKEDIT"    /* the segment containing all structs */
                    /* created and maintained by the link */
                    /* editor.  Created with -seglinkedit */
                    /* option to ld(1) for MH_EXECUTE and */
                    /* FVMLIB file types only */

#define     SEG_LINKINFO    "__LINKINFO"    /* the segment overlapping with linkedit */
                    /* containing linking information */

#define     SEG_UNIXSTACK    "__UNIXSTACK"    /* the unix stack segment */

#define     SEG_IMPORT    "__IMPORT"    /* the segment for the self (dyld) */
                    /* modifing code stubs that has read, */
                    /* write and execute permissions */

struct z_mach_header_64
{
    z_uint32    magic;          /* mach magic number identifier */
    z_int32     cputype;        /* cpu specifier */
    z_int32     cpusubtype;     /* machine specifier */
    z_uint32    filetype;       /* type of file */
    z_uint32    ncmds;          /* number of load commands */
    z_uint32    sizeofcmds;     /* the size of all the load commands */
    z_uint32    flags;          /* flags */
    z_uint32    reserved;       /* reserved */
};

struct z_load_command
{
    z_uint32 cmd;               /* type of load command */
    z_uint32 cmdsize;           /* total size of command in bytes */
};

struct segment_command_64
{
    /* for 64-bit architectures */
    z_uint32    cmd;            /* LC_SEGMENT_64 */
    z_uint32    cmdsize;        /* includes sizeof section_64 structs */
    z_int8      segname[16];    /* segment name */
    z_uint64    vmaddr;         /* memory address of this segment */
    z_uint64    vmsize;         /* memory size of this segment */
    z_uint64    fileoff;        /* file offset of this segment */
    z_uint64    filesize;       /* amount to map from the file */
    z_int32     maxprot;        /* maximum VM protection */
    z_int32     initprot;       /* initial VM protection */
    z_uint32    nsects;         /* number of sections in segment */
    z_uint32    flags;          /* flags */
};

struct section_64
{
    /* for 64-bit architectures */
    z_int8      sectname[16];   /* name of this section */
    z_int8      segname[16];    /* segment this section goes in */
    z_uint64    addr;           /* memory address of this section */
    z_uint64    size;           /* size in bytes of this section */
    z_uint32    offset;         /* file offset of this section */
    z_uint32    align;          /* section alignment (power of 2) */
    z_uint32    reloff;         /* file offset of relocation entries */
    z_uint32    nreloc;         /* number of relocation entries */
    z_uint32    flags;          /* flags (section type and attributes)*/
    z_uint32    reserved1;      /* reserved (for offset or index) */
    z_uint32    reserved2;      /* reserved (for count or sizeof) */
    z_uint32    reserved3;      /* reserved */
};

#endif /* z_machO_h */
