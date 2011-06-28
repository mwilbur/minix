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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "elfparse.h"
#include "elf.h"

elf32_ehdr_t* get_elf32_ehdr(char* buf, elf32_ehdr_t* ehdr)
{
  if(!ehdr)
    /* allocate space for program headers and copy data */
    ehdr = (elf32_ehdr_t*)malloc(sizeof(elf32_ehdr_t));

  if(!ehdr) {
    fprintf(stderr,"Failed to allocate space for phdrs\n");
    return 0;
  }

  /* copy header from file */
  memcpy(ehdr, buf, sizeof(elf32_ehdr_t));

  return ehdr;
}

elf32_ehdr_t* read_elf32_ehdr(char* filename, elf32_ehdr_t* ehdr)
{
  int fd = 0;
  int n = 0;

  if(!ehdr)
    /* allocate space for program headers if needed */
    ehdr = (elf32_ehdr_t*)malloc(sizeof(elf32_ehdr_t));

  if(!ehdr) {
    perror(__FUNCTION__);

    return 0;
  }

  if((fd = open(filename,O_RDONLY))<0) {
    perror(__FUNCTION__);
    free(ehdr);

    return 0;
  }

  n = read(fd, ehdr, sizeof(elf32_ehdr_t));

  close(fd);

  if( n < 0 || n != sizeof(elf32_ehdr_t)) {
    perror(__FUNCTION__);
    free(ehdr);

    return 0;
  }

  return ehdr;
}

elf32_phdr_t* get_elf32_phdrs(char* buf, elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs)
{
  /* allocate space if needed */
  if(!phdrs) {
    phdrs = (elf32_phdr_t*)malloc(ehdr->e_phnum*sizeof(elf32_phdr_t));
    if (!phdrs) {
      fprintf(stderr,"Failed to allocate space for phdrs\n");
      return 0;
    }
  }

  memcpy(phdrs, buf + ehdr->e_phoff, ehdr->e_phnum*sizeof(elf32_phdr_t));

  return phdrs;
}

elf32_phdr_t* read_elf32_phdrs(char* filename, elf32_ehdr_t* ehdr, elf32_phdr_t* phdrs)
{
  int fd = 0;
  int n = 0;
  int phdrs_sz = 0;

  if (!ehdr) {
    fprintf(stderr,"Empty ELF32 header!\n");
    return 0;
  }

  if (!is_elf32((char*)ehdr)) {
    fprintf(stderr,"Incorrect ELF32 header\n");
    return 0;
  }

  if (ehdr->e_phnum)
    phdrs_sz = ehdr->e_phnum*sizeof(elf32_phdr_t);
  else {
    fprintf(stderr,"Missing program headers\n");
    return 0;
  }

  if (!phdrs)
    phdrs = (elf32_phdr_t*)malloc(phdrs_sz);

  if (!phdrs) {
    perror(__FUNCTION__);
    return 0;
  }

  if ((fd = open(filename,O_RDONLY)) < 0) {
    perror(__FUNCTION__);
    return 0;
  }

  if(lseek(fd, ehdr->e_phoff, SEEK_SET) < 0) {
    perror(__FUNCTION__);
    free(phdrs);
    return 0;
  }

  if ((n = read(fd, phdrs, phdrs_sz)) < 0) {
    perror(__FUNCTION__);
    free(phdrs);
    close(fd);

    return 0;
  }

  if (n != phdrs_sz) {
    fprintf(stderr,"Wrong size: want %d but get %d bytes\n", phdrs_sz, n);
    free(phdrs);
  }

  close(fd);

  return phdrs;
}

elf32_shdr_t* get_elf32_shdrs(char* buf, elf32_ehdr_t* ehdr, elf32_shdr_t* shdrs)
{
  /* allocate space if needed */
  if(!shdrs) {
    shdrs = (elf32_shdr_t*)malloc(ehdr->e_shnum*sizeof(elf32_shdr_t));
    if (!shdrs) {
      fprintf(stderr,"Failed to allocate space for shdrs\n");
      return 0;
    }
  }
  memcpy(shdrs, buf + ehdr->e_shoff, ehdr->e_shnum*sizeof(elf32_shdr_t));

  return shdrs;
}

elf32_shdr_t* read_elf32_shdrs(char* filename, elf32_ehdr_t* ehdr, elf32_shdr_t* shdrs)
{
  int fd = 0;
  int n = 0;
  int shdrs_sz = 0;

  if (!ehdr) {
    fprintf(stderr,"Empty ELF32 header!\n");
    return 0;
  }

  if (!is_elf32((char*)ehdr)) {
    fprintf(stderr,"Incorrect ELF32 header\n");
    return 0;
  }

  if (ehdr->e_shnum)
    shdrs_sz = ehdr->e_shnum*sizeof(elf32_shdr_t);
  else {
    fprintf(stderr,"Missing section headers\n");
    return 0;
  }

  if (!shdrs)
    shdrs = (elf32_shdr_t*)malloc(shdrs_sz);

  if (!shdrs) {
    perror(__FUNCTION__);
    return 0;
  }

  if ((fd = open(filename,O_RDONLY)) < 0) {
    perror(__FUNCTION__);
    return 0;
  }

  if(lseek(fd, ehdr->e_shoff, SEEK_SET) < 0) {
    perror(__FUNCTION__);
    free(shdrs);
    return 0;
  }

  if ((n = read(fd, shdrs, shdrs_sz)) < 0) {
    perror(__FUNCTION__);
    free(shdrs);
    close(fd);

    return 0;
  }

  if(n != shdrs_sz) {
    fprintf(stderr,"Wrong size: want %d but get %d bytes\n", shdrs_sz, n);
    free(shdrs);
  }

  close(fd);

  return shdrs;
}
