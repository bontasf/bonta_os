#include "elf_parser.h"

#include "utils.h"

typedef struct _ELF64_HEADER
{
    struct
    {
        UINT8 ei_mag0;
        UINT8 ei_mag1;
        UINT8 ei_mag2;
        UINT8 ei_mag3;
        UINT8 ei_class;
        UINT8 ei_data;
        UINT8 ei_version;
        UINT8 ei_osabi;
        UINT8 ei_abiversion;
        UINT8 ei_pad[7];
    } e_ident;

    UINT16 e_type;
    UINT16 e_machine;
    UINT32 e_version;
    UINT64 e_entry;
    UINT64 e_phoff;
    UINT64 e_shoff;
    UINT32 e_flags;
    UINT16 e_ehsize;
    UINT16 e_phentsize;
    UINT16 e_phnum;
    UINT16 e_shentsize;
    UINT16 e_shnum;
    UINT16 e_shstrndx;
} __attribute__((packed)) ELF64_HEADER;

typedef struct _ELF64_PROGRAM_HEADER
{
    UINT32 p_type;
    UINT32 p_flags;
    UINT64 p_offset;
    UINT64 p_vaddr;
    UINT64 p_paddr;
    UINT64 p_filesz;
    UINT64 p_memsz;
    UINT64 p_align;
} __attribute__((packed)) ELF64_PROGRAM_HEADER;

typedef enum _ELF_EHEADER_TYPE
{
    ET_EXEC = 0x2,
    ET_DYN = 0x3,
} ELF_EHEADER_TYPE;

typedef enum _ELF_PHEADER_TYPE
{
    PT_NULL = 0x0,
    PT_LOAD = 0x1,
} ELF_PHEADER_TYPE;

STATUS API ParseElf64(IN VOID *FileBuffer, OUT VOID **ElfEntry, IN ALLOCATE_MEMORY AllocateMemory)
{
    STATUS Status = E_OK;
    VOID *ElfEntryOutput = NULL_PTR;

    if (NULL_PTR == FileBuffer)
    {
        Status = E_NOT_OK;
        goto Cleanup;
    }

    if (NULL_PTR == ElfEntry)
    {
        Status = E_NOT_OK;
        goto Cleanup;
    }

    if (NULL_PTR != *ElfEntry)
    {
        Status = E_NOT_OK;
        goto Cleanup;
    }

    ELF64_HEADER *Header = (ELF64_HEADER *)FileBuffer;
    if (ET_DYN != Header->e_type)
    {
        Status = E_NOT_OK;
        goto Cleanup;
    }

    ELF64_PROGRAM_HEADER *ProgramHeader = (ELF64_PROGRAM_HEADER *)((UINT8 *)Header + Header->e_phoff);
    UINT64 MaxAlignment = 4096;
    UINT64 MemoryMin = 0xFFFFFFFFFFFFFFFF;
    UINT64 MemoryMax = 0;

    for (UINT64 i = 0; i < Header->e_phnum; ++i, ++ProgramHeader)
    {
        if (PT_LOAD == ProgramHeader->p_type)
        {
            if (MaxAlignment < ProgramHeader->p_align)
            {
                MaxAlignment = ProgramHeader->p_align;
            }

            UINT64 HeaderBegin = ProgramHeader->p_vaddr;
            UINT64 HeaderEnd = ProgramHeader->p_vaddr + ProgramHeader->p_memsz + MaxAlignment - 1;

            HeaderBegin &= ~(MaxAlignment - 1);
            HeaderEnd &= ~(MaxAlignment - 1);

            if (HeaderBegin < MemoryMin)
            {
                MemoryMin = HeaderBegin;
            }

            if (HeaderEnd > MemoryMax)
            {
                MemoryMax = HeaderEnd;
            }
        }
    }

    UINT64 MaxMemoryNeeded = MemoryMax - MemoryMin;
    Status = AllocateMemory(&ElfEntryOutput, MaxMemoryNeeded);
    if (E_OK != Status)
    {
        goto Cleanup;
    }

    MemorySet(ElfEntryOutput, 0, MaxMemoryNeeded);

    ProgramHeader = (ELF64_PROGRAM_HEADER *)((UINT8 *)Header + Header->e_phoff);
    for (UINT16 i = 0; i < Header->e_phnum; ++i, ++ProgramHeader)
    {
        if (PT_LOAD == ProgramHeader->p_type)
        {
            UINT64 RelativeOffset = ProgramHeader->p_vaddr - MemoryMin;
            UINT8 *Destination = (UINT8 *)ElfEntryOutput + RelativeOffset;
            UINT8 *Source = (UINT8 *)FileBuffer + ProgramHeader->p_offset;
            UINT32 Size = ProgramHeader->p_filesz;

            MemoryCopy(Source, Destination, Size);
        }
    }

Cleanup:
    *ElfEntry = (VOID *)((UINT8 *)ElfEntryOutput + (Header->e_entry - MemoryMin));
    return Status;
}
