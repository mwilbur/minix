/*
 * Copyright (c) 2008, Ladislav Klenovic < klenovic@nucleonsoft.com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *
 *      This product includes software developed by Ladislav Klenovic.
 *      Contact: klenovic@nucleonsoft.com
 *
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#ifndef __NUCS_ELFPARSE_H
#define __NUCS_ELFPARSE_H

#include "elf.h"
#include "exec.h"

typedef Elf32_Ehdr elf32_ehdr_t;
typedef Elf32_Phdr elf32_phdr_t;
typedef Elf32_Shdr elf32_shdr_t;

static inline int is_elf (char* e)
{
  if (e[EI_MAG0] != ELFMAG0 ||
      e[EI_MAG1] != ELFMAG1 ||
      e[EI_MAG2] != ELFMAG2 ||
      e[EI_MAG3] != ELFMAG3) {
    /* it's not an ELF file */
    return 0;
  }

  /* it's an ELF file */
  return 1;
}

static inline int is_elfclass32(char* e)
{
  if(e[EI_CLASS] != ELFCLASS32)
    return 0;

  return 1;
}

#define is_32bit(e) is_elfclass32(e)

static inline int is_elf32(char* e)
{
  if(is_elf(e) && is_elfclass32(e))
    return 1;

  return 0;
}

/** Get ELF header structure from buffer. */
elf32_ehdr_t* get_elf32_ehdr(char* buf, elf32_ehdr_t* ehdr);

/** Get ELF header structure from file. */
elf32_ehdr_t* read_elf32_ehdr(char* filename, elf32_ehdr_t* ehdr);

/** Get ELF program headers from buffer. */
elf32_phdr_t* get_elf32_phdrs(char* buf, elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs);

/** Get ELF program headers from file. */
elf32_phdr_t* read_elf32_phdrs(char* filename, elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs);

/** Get ELF section headers from buffer. */
elf32_shdr_t* get_elf32_shdrs(char* buf, elf32_ehdr_t* ehdr, elf32_shdr_t* shdrs);

/** Get ELF section headers from file. */
elf32_shdr_t* read_elf32_shdrs(char* filename, elf32_ehdr_t* ehdr, elf32_shdr_t* shdrs);

static inline void dump_ehdr(elf32_ehdr_t* eh)
{
  int i=0;

  printf("e_ident: ");
  for(i = 0; i < EI_NIDENT; i++) {
    printf("0x%x ",eh->e_ident[i]);     /* Magic number and other info */
  }
  putchar('\n');

  printf("type: 0x%x ", eh->e_type);            /* Object file type */
  printf("machine: 0x%x ", eh->e_machine);      /* Architecture */
  printf("version: 0x%x ", eh->e_version);      /* Object file version */
  printf("entry: 0x%x ", eh->e_entry);          /* Entry point virtual address */
  printf("phoff: 0x%x\n", eh->e_phoff);         /* Program header table file offset */
  printf("shoff: 0x%x ", eh->e_shoff);          /* Section header table file offset */
  printf("flags: 0x%x ", eh->e_flags);          /* Processor-specific flags */
  printf("ehsize: 0x%x ", eh->e_ehsize);         /* ELF header size in bytes */
  printf("phentsize: 0x%x\n", eh->e_phentsize); /* Program header table entry size */
  printf("phnum: 0x%x ", eh->e_phnum);          /* Program header table entry count */
  printf("shentsize: 0x%x ", eh->e_shentsize);  /* Section header table entry size */
  printf("shnum: 0x%x ", eh->e_shnum);          /* Section header table entry count */
  printf("shstrndx: 0x%x\n", eh->e_shstrndx);   /* Section header string table index */

  return;
}

static inline void dump_phdr(elf32_phdr_t* ph)
{

  printf("type: 0x%x ", ph->p_type);     /* Segment type */
  printf("offset: 0x%x ", ph->p_offset); /* Segment file offset */
  printf("vaddr: 0x%x ", ph->p_vaddr);   /* Segment virtual address */
  printf("paddr: 0x%x\n", ph->p_paddr);  /* Segment physical address */
  printf("filesz: 0x%x ", ph->p_filesz); /* Segment size in file */
  printf("memsz: 0x%x ", ph->p_memsz);   /* Segment size in memory */
  printf("flags: 0x%x ", ph->p_flags);   /* Segment flags */
  printf("align: 0x%x\n", ph->p_align);  /* Segment alignment */

  return;
}

static inline void dump_shdr(elf32_shdr_t* sh)
{
  printf("name: 0x%x ", sh->sh_name);           /* Section name (string tbl index) */
  printf("type: 0x%x ", sh->sh_type);           /* Section type */
  printf("flags: 0x%x ", sh->sh_flags);         /* Section flags */
  printf("addr: 0x%x ", sh->sh_addr);           /* Section virtual addr at execution */
  printf("offset: 0x%x\n", sh->sh_offset);      /* Section file offset */
  printf("size: 0x%x ", sh->sh_size);           /* Section size in bytes */
  printf("link: 0x%x ", sh->sh_link);           /* Link to another section */
  printf("info: 0x%x ", sh->sh_info);           /* Additional section information */
  printf("addralign: 0x%x ", sh->sh_addralign); /* Section alignment */
  printf("entsize: 0x%x\n", sh->sh_entsize);    /* Entry size if section holds table */

  return;
}

#endif /* !__NUCS_ELFPARSE_H */
