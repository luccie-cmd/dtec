#include <stdio.h>
#include <stdint.h>

enum{
    LITTLE_ENDIAN=1,
    BIG_ENDIAN=2,
} Endianness;

enum{
    BITS_32=1,
    BITS_64,
} Data;

enum{
    SYSTEM_V=0,
} Abi;

enum{
    AMD64=0x3E,
} Machine;

uint8_t *header = "\x7F" "ELF";
uint64_t padding = 0;
uint64_t endianness = LITTLE_ENDIAN;
uint64_t data = BITS_64;
uint64_t version = 1;
uint64_t os_abi = SYSTEM_V;
uint64_t os_abi_version = 0;
uint64_t type = 2; // exe
uint64_t machine = AMD64;
uint64_t elf_version = 1;
uint64_t entry = 4198400;
uint64_t phoff = 64;
uint64_t shoff = 8496;
uint64_t flags = 0;
uint64_t elf_header_size = 64;
uint64_t phentsize = 56;
uint64_t phnum = 2;
uint64_t shentsize = 64;
uint64_t shnum = 5;
uint64_t shstrndx = 4;

void write_elf_header(FILE *f){
    fwrite(header, 1, 4, f);
    fwrite(&data, 1, 1, f);
    fwrite(&endianness, 1, 1, f);
    fwrite(&version, 1, 1, f);
    fwrite(&os_abi, 1, 1, f);
    fwrite(&os_abi_version, 1, 1, f);
    fwrite(&padding, 1, 7, f);
    fwrite(&type, 1, 2, f);
    fwrite(&machine, 1, 2, f);
    fwrite(&elf_version, 1, 4, f);
    switch(data){
        case BITS_64: {
            fwrite(&entry, 1, 8, f);
            fwrite(&phoff, 1, 8, f);
            fwrite(&shoff, 1, 8, f);
        } break;
        case BITS_32: {
            fwrite(&entry, 1, 4, f);
            fwrite(&phoff, 1, 4, f);
            fwrite(&shoff, 1, 4, f);
        } break;
    }
    fwrite(&flags, 1, 4, f);
    fwrite(&elf_header_size, 1, 2, f);
    fwrite(&phentsize, 1, 2, f);
    fwrite(&phnum, 1, 2, f);
    fwrite(&shentsize, 1, 2, f);
    fwrite(&shnum, 1, 2, f);
    fwrite(&shstrndx, 1, 2, f);
}

enum{
    PF_X=0x1, //	Executable segment.
    PF_W=0x2, //	Writeable segment.
    PF_R=0x4, //	Readable segment.
} P_Flags;

int prog_header_count_written = 0;

void write_prog_header(FILE *f, uint64_t p_flags, uint64_t p_filesz){
    uint64_t p_type = 1;
    uint64_t p_offset = 0+(prog_header_count_written*4096);
    uint64_t p_vaddr = 4194304+(prog_header_count_written*4096);
    uint64_t p_paddr = 4194304+(prog_header_count_written*4096);
    uint64_t p_memsz = p_filesz;
    uint64_t p_align = 4096;
    fwrite(&p_type, 1, 4, f);
    if(data == BITS_64){
        fwrite(&p_flags, 1, 4, f);
    }
    switch(data){
        case BITS_64: {
            fwrite(&p_offset, 1, 8, f);
            fwrite(&p_vaddr, 1, 8, f);
            fwrite(&p_paddr, 1, 8, f);
            fwrite(&p_filesz, 1, 8, f);
            fwrite(&p_memsz, 1, 8, f);
        } break;
        case BITS_32: {
            fwrite(&p_offset, 1, 4, f);
            fwrite(&p_vaddr, 1, 4, f);
            fwrite(&p_paddr, 1, 4, f);
            fwrite(&p_filesz, 1, 4, f);
            fwrite(&p_memsz, 1, 4, f);
        } break;
    }
    if(data == BITS_32){
        fwrite(&p_flags, 1, 4, f);
        fwrite(&p_align, 1, 4, f);
    } else{
        fwrite(&p_align, 1, 8, f);
    }
    prog_header_count_written++;
}

int main(){
    FILE* f = fopen("Myelf.o", "wb");
    write_elf_header(f);
    fseek(f, phoff, SEEK_SET);
    write_prog_header(f, PF_R, 232);
    write_prog_header(f, PF_R + PF_X, 39);
    fseek(f, entry&0xFFFF, SEEK_SET);

    return 0;
}