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

/* 
 * Create minix3 a.out header from elf file 
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "elfparse.h"
#include "exec.h"
#include <minix/a.out.h>

#define DO_TRACE     0

void usage(void)
{
  fprintf(stderr,
    "Usage: ehdr2ahdr [-c cpu] [-d faout] [-h] -i input -o output -s stackheap\n"
    " -c cpu: cpu type (386 or 8086)\n"
    " -d faout: dump file in aout format\n"
    " -f list of flags: list flags separated by `,' (see a.out.h for posible flags)\n"
    " -i input: input file (ELF32 binary format)\n"
    " -o output: output file (minix3's aout header)\n"
    " -s stackheap: stack + heap size in bytes\n"
    " -h : help\n");

  return;
}

/* Report problems. */
void report(char *problem, char *message)
{
  fprintf(stderr, "%s:\n", problem);
  fprintf(stderr, "   %s\n\n", message);
}

void dump_exec_elf32(exec_elf32_t* e)
{
  printf("typetype=0x%x  ",e->type);
  printf("version=0x%x\n",e->version);
  printf("text_pstart=0x%x  ",e->text_pstart);
  printf("text_vstart=0x%x  ",e->text_vstart);
  printf("text_size=0x%x\n",e->text_size);
  printf("data_pstart=0x%x  ",e->data_pstart);
  printf("data_vstart=0x%x  ",e->data_vstart);
  printf("data_size=0x%x\n",e->data_size);
  printf("bss_pstart=0x%x  ",e->bss_pstart);
  printf("bss_vstart=0x%x  ",e->bss_vstart);
  printf("bss_size=0x%x\n",e->bss_size);
  printf("initial_ip=0x%x  ",e->initial_ip);
  printf("flags=0x%x  ",e->flags);
  printf("label=0x%x  ",e->label);
  printf("cmdline_offset=0x%x\n",e->cmdline_offset);

  return;
}

void dump_aout(struct exec* ahdr)
{
  printf("magic: 0x%x 0x%x \n",ahdr->a_magic[0],ahdr->a_magic[1]);     /* magic number */
  printf("flags: 0x%x \n",ahdr->a_flags);        /* flags, see below */
  printf("cpu: 0x%x \n",ahdr->a_cpu);          /* cpu id */
  printf("hdrlen: 0x%x \n",ahdr->a_hdrlen);       /* length of header */
  printf("unused: 0x%x \n",ahdr->a_unused);       /* reserved for future use */
  printf("version: 0x%x\n\n",ahdr->a_version);     /* version stamp (not used at present) */
  printf("text: 0x%x \n",(unsigned) ahdr->a_text);         /* size of text segement in bytes */
  printf("data: 0x%x \n",(unsigned) ahdr->a_data);         /* size of data segment in bytes */
  printf("bss: 0x%x \n",(unsigned) ahdr->a_bss);          /* size of bss segment in bytes */
  printf("entry: 0x%x \n",(unsigned) ahdr->a_entry);        /* entry point */
  printf("total: 0x%x \n",(unsigned) ahdr->a_total);        /* total memory allocated */
  printf("syms: 0x%x\n\n",(unsigned) ahdr->a_syms);         /* size of symbol table */

  /* SHORT FORM ENDS HERE */
  if(ahdr->a_hdrlen > 0x20) {
    printf("trsize: 0x%x \n", (unsigned) ahdr->a_trsize);       /* text relocation size */
    printf("drsize: 0x%x \n",(unsigned) ahdr->a_drsize);       /* data relocation size */
    printf("tbase: 0x%x \n",(unsigned) ahdr->a_tbase);        /* text relocation base */
    printf("dbase: 0x%x\n\n",(unsigned) ahdr->a_dbase);        /* data relocation base */
  }

  return;
}

int create_exec_elf32(elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs, elf32_shdr_t* shdrs, struct exec_elf32* exec);

/* default header length */
#define HDRLEN     0x20
#define MAX_DIGITS   20
#define MAX_TOKENS    8

/* Main program. */
int main(int argc, char **argv)
{
  struct exec ahdr;   // minix aout header
  struct exec_elf32 input_exec_elf32;
  struct stat st_input;
  elf32_ehdr_t ehdr;
  elf32_phdr_t* phdrs = 0;
  elf32_shdr_t* shdrs = 0;
  int fd_in;
  int fd_out;
  int fd_aout;
  char* input = 0;
  char* output = 0;
  char* faout = 0;
  char strnum[MAX_DIGITS]; 
  char* strflags[MAX_TOKENS]; 
  unsigned flags = A_EXEC;
  int flags_yes = 0;
  unsigned char cpu = A_I80386;
  int stackheap = -1;
  int hdrlen = HDRLEN;
  int opt;
  int i=0;
  int n=0;

  while ((opt = getopt(argc, argv,"c:d:f:hi:l:o:s:")) != -1) {
    switch (opt) {
      case 'c': /* cpu type */
        if(!strncmp(optarg,"i386",4))
          cpu = A_I80386;
        if(!strncmp(optarg,"i8086",5))
          cpu = A_I8086;
        break;

      case 'd': /* dump aout header */
        faout = optarg;
        break;

      case 'f': /* flags */
        memset(strflags,0,MAX_TOKENS);
        flags = 0x00;
        /* get the first token */
        strflags[0] = strtok(optarg," ,");

        /* parse the rest of string */
        while ((++i <= MAX_TOKENS) && ((strflags[i] = strtok (0," ,")) != 0));

        n=i;
        unsigned flag = 0x00;

        for(i=0; i<n; i++) {
          if(strlen(strflags[i]) > 2 && strflags[i][0] == '0' && strflags[i][1] == 'x')
            sscanf(strflags[i],"%x",&flag);
          else
            flag = atoi(strflags[i]);
          flags |= flag;
        }
        break;

      case 'h':
        usage();
        exit(0);

      case 'i': /* input */
        input = optarg;
        break;

      case 'l': /* header length (default 0x20) */
        memset(strnum,0,MAX_DIGITS);
        if(strlen(optarg) > 2 && optarg[0] == '0' && optarg[1] == 'x')
          sscanf(optarg,"%x",&hdrlen);
        else
          hdrlen = atoi(optarg);

        break;

      case 'o': /* output */
        output = optarg;
        break;

      case 's': /* stack + heap size */
	{
		char * mulstr;
		stackheap = strtoul(optarg, &mulstr, 0);
		if (mulstr[0] != 0) {
			switch (mulstr[0]) {
				case 'k':
					stackheap <<= 10;
					break;
				case 'M':
					stackheap <<= 20;
					break;
				default:
					goto wrong_size;
			}
			if (mulstr[1] != 0) {
				switch(mulstr[1]) {
					case 'w':
						stackheap *= 4; /* assuming 32bits */
						break;
					case 'b':
						break;
					default:
						goto wrong_size;
				}
			}
		}
		break;

wrong_size:
		fprintf(stderr, "Unrecognized size modifier\n");
		usage();
		exit(1);
	}

        break;

      default: /* '?' */
        usage();
        exit(1);
    }
  }

  if(faout) {
    if((fd_aout = open(faout, O_RDONLY))<0) {
      perror("open");
      exit(1);
    }

    if ((n = read(fd_aout, &ahdr, sizeof(ahdr))) < 0) {
      perror("read");
    }

    close(fd_aout);

    if (n != sizeof(ahdr)) {
      fprintf(stderr,"Wrong header size (want %d got %d\n",sizeof(ahdr),n);
    }

    dump_aout(&ahdr);

    return 0;
  }

  if(!input || !output || stackheap<0) {
    usage();
    exit(1);
  }

  if (stat(input, &st_input) != 0) {
    perror("stat");
    exit(1);
  }

  if((fd_in = open(input, O_RDONLY))<0) {
    report(input,"Couldn't open input file");
    exit(1);
  }

  if((fd_out = open(output, O_CREAT|O_RDWR|O_TRUNC, S_IFREG|S_IRUSR|S_IWUSR|S_IWUSR))<0) {
    report(output,"Couldn't open output file");
    exit(1);
  }

  read_elf32_ehdr(input, &ehdr);

#if DO_TRACE == 1
  printf("---[ELF header]---\n");
  dump_ehdr(&ehdr);
#endif

  phdrs = read_elf32_phdrs(input, &ehdr, phdrs);

#if DO_TRACE == 1
  for(i=0; i<ehdr.e_phnum; i++) {
    printf("---[%d. program header]---\n",i);
    dump_phdr(&phdrs[i]);
  }
#endif

  shdrs = read_elf32_shdrs(input, &ehdr, shdrs);

#if DO_TRACE == 1
  for(i=0; i<ehdr.e_shnum; i++) {
    printf("---[%d. section header]---\n",i);
    dump_shdr(&shdrs[i]);
  }
#endif

  create_exec_elf32(&ehdr, phdrs, shdrs, &input_exec_elf32);

#if DO_TRACE == 1
  dump_exec_elf32(&input_exec_elf32);
#endif

  /* Build a.out header and write to output */
  memset(&ahdr, 0, sizeof(ahdr));
  ahdr.a_magic[0] = A_MAGIC0;
  ahdr.a_magic[1] = A_MAGIC1;
  ahdr.a_flags    = flags;
  ahdr.a_cpu      = cpu;
  ahdr.a_hdrlen   = hdrlen;
  ahdr.a_text     = input_exec_elf32.text_size;
  ahdr.a_data     = input_exec_elf32.data_size;
  ahdr.a_bss      = input_exec_elf32.bss_size;

  if (!(flags & A_SEP)) /* Common I&D */
    ahdr.a_total = ahdr.a_text + ahdr.a_data + ahdr.a_bss + stackheap;
  else /* separate I&D */
    ahdr.a_total = ahdr.a_data + ahdr.a_bss + stackheap;

  ahdr.a_entry = input_exec_elf32.initial_ip;

  if (flags & A_NSYM) {
    ahdr.a_syms = input_exec_elf32.symtab_size;
  } else {
    ahdr.a_syms = 0;
  }

  write(fd_out, &ahdr, hdrlen);

  close(fd_in);
  close(fd_out);

  return 0;
}

int create_exec_elf32(elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs, elf32_shdr_t* shdrs, struct exec_elf32* exec)
{
  int i = 0;

  memset(exec, 0, sizeof(*exec));

  // Initialize a local bootinfo record
  memset ((void*) exec, 0, sizeof (*exec));

  exec->type = NEXEC;
  exec->version = 1;
  exec->initial_ip = ehdr->e_entry;
  exec->offset_next = sizeof (*exec);
  exec->flags = ehdr->e_ident[4];

  if (ehdr->e_phoff != 0 && ehdr->e_shoff == 0) {
    /*
     * We only have a program headers.  Walk all program headers
     * and try to figure out what the sections arehdr->
     */

    for (i = 0; i < ehdr->e_phnum; i++) {
      elf32_phdr_t* ph = &phdrs[i];

      // Assume that a segment without write permissions is .text
      if ((ph->p_flags & PF_W) == 0) {
        exec->text_pstart = ph->p_paddr;
        exec->text_vstart = ph->p_paddr;
        exec->text_size   = ph->p_filesz;
      } else {
        exec->data_pstart = ph->p_paddr;
        exec->data_vstart = ph->p_paddr;
        exec->data_size   = ph->p_filesz;
      }
      if (ph->p_memsz > ph->p_filesz) {
        // Looks like a bss section
        exec->bss_pstart = ph->p_paddr + ph->p_filesz;
        exec->bss_vstart = ph->p_vaddr + ph->p_filesz;
        exec->bss_size   = ph->p_memsz - ph->p_filesz;
      }
    }

    return 0;
  }

  /*
   * If we have the section headers we can try to figure out the
   * real .text, .data, and .bss segments by inspecting the section
   * type and flags.
   */
  unsigned int tlow = ~0UL, thigh = 0;
  unsigned int dlow = ~0UL, dhigh = 0;
  unsigned int blow = ~0UL, bhigh = 0;

  for (i = 0; i < ehdr->e_shnum; i++) {
    elf32_shdr_t* sh = &shdrs[i];

    if (sh->sh_type == SHT_PROGBITS) {
      /* segments with "x" flag */
      if ((sh->sh_flags & SHF_ALLOC) && (sh->sh_flags & SHF_EXECINSTR))  {
        if (sh->sh_addr < tlow)
          tlow = sh->sh_addr;

        if (sh->sh_addr + sh->sh_size > thigh)
          thigh = sh->sh_addr + sh->sh_size;

      } else if ((sh->sh_flags & SHF_ALLOC) && (~sh->sh_flags & SHF_EXECINSTR)) {
        /* sections .data and .rodata */
        if (sh->sh_addr < dlow)
          dlow = sh->sh_addr;

        if (sh->sh_addr + sh->sh_size > dhigh)
          dhigh = sh->sh_addr + sh->sh_size;
      }
    } else if (sh->sh_type == SHT_NOBITS) {
      /* Assume that all nobits sections are .bss */
      if (sh->sh_addr < blow)
        blow = sh->sh_addr;

      if (sh->sh_addr + sh->sh_size > bhigh)
        bhigh = sh->sh_addr + sh->sh_size;
    } else if (sh->sh_type == SHT_SYMTAB) {
      /* Symbol table */
      exec->symtab_size = sh->sh_size;
    } else if (sh->sh_type == SHT_STRTAB) {
      /* String table */
      exec->strtab_size = sh->sh_size;
    }
  }
  /*
   * Translate the virtual addresses of the sections to physical
   * addresses using the segments in the program header.
   */
#define INSEGMENT(a)    (a >= ph->p_vaddr && a < (ph->p_vaddr + ph->p_memsz))
#define PADDR(a)        (ph->p_paddr + a - ph->p_vaddr)

  for (i = 0; i < ehdr->e_phnum; i++) {
    elf32_phdr_t* ph = &phdrs[i];

    if (INSEGMENT (tlow)) {
      exec->text_pstart = PADDR (tlow);
      exec->text_vstart = tlow;
      exec->text_size = thigh - tlow;
    }

    if (INSEGMENT (dlow)) {
      exec->data_pstart = PADDR (dlow);
      exec->data_vstart = dlow;
      exec->data_size   = dhigh - dlow;
    }

    if (INSEGMENT (blow)) {
      exec->bss_pstart = PADDR (blow);
      exec->bss_vstart = blow;
      exec->bss_size   = bhigh - blow;
    }
  }

  return 0;
}
