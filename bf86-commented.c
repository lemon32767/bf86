#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR(s) (fflush(stdout), fprintf(stderr, "  ERROR: %s\n", s), exit(127))

static char prg[16*1024]; //16kB stripped of non instruction characters should be enough
static int prg_len = 0;

int main(int argc, char** argv) {
  // size in bytes of the assembly generated for each bf instruction. every other character is zero
  const char ins_sizes[256] = {
    ['+'] = 2, ['-'] = 2, ['>'] = 1, ['<'] = 1, ['['] = 9, [']'] = 9, ['.'] = 8, [','] = 8
  };
  
  //for the elf header, essentially the size of the executable section (p_filesz + p_memsz)
  unsigned memsz = 64;

  if (argc > 1) for (char* s = argv[1]; *s; s++) { //read program from given arg
    if (prg_len >= sizeof prg) ERR("input is too large");
    int sz = ins_sizes[(int)*s];
    if (sz != 0) prg[prg_len++] = *s, memsz += sz;
  } else for (int c; (c = getchar()) != EOF;) { //read from stdin
    if (prg_len >= sizeof prg) ERR("input is too large");
    int sz = ins_sizes[c];
    if (sz != 0) prg[prg_len++] = c, memsz += sz;
  }
  if ((prg_len == 0 || (prg_len == 1 && *prg == '.')) && argc > 1) fprintf(stderr, "(stderr) did you mean `%s < \"%s\"`?\n", *argv, argv[1]);

  //ELF header

  const unsigned char header0[] =
    "\x7F""ELF\1\1\1\0\0\0\0\0\0\0\0\0\2\0\3\0\1\0\0\0\0"
    "\x90\4\x8""4\0\0\0\xC0\x10\0\0\0\0\0\0""4\0"" \0\2\0"
    "(\0\1\0\4\0\1\0\0\0\0\0\0\0\0\x80\0\x8\0\x80\4\x8"
    "t\0\0\0""t\0\0\0\4\0\0\0\0\x10\0\0\1\0\0\0\0\x10\0\0"
    "\0\x90\4\x8\0\x90\4\x8";
  fwrite(header0, 1, sizeof header0 - 1, stdout);

  fwrite(&memsz, 4, 1, stdout); //p_filesz
  fwrite(&memsz, 4, 1, stdout); //p_memsz

  const unsigned char header1[] = "\5\0\0\0\0\x10\0";
  fwrite(header1, 1, sizeof header1 - 1, stdout);

  for (int i = 0; i < 3981; i++) putchar(0); //padding stuff

  #define ASM(x) fwrite((const unsigned char[]){x}, 1, -1+sizeof(const unsigned char[]){x}, stdout)

  //initialize
  
  //obtaining memory buffer for the tape using mmap(2)
  ASM("\xB8\xC0\0\0\0"); // mov  eax, $C0            ; eax = mmap2 Nr. = 0xC0
  ASM("\x31\xDB");       // xor  ebx, ebx            ; ebx = addr = 0
  ASM("\xB9\0\x78\0\0"); // mov  ecx, $4000          ; ecx = size = 0x7800 = 30 kB ~ 30_000 cells
  ASM("\xBA\3\0\0\0");   // mov  edx, 3              ; edx = prot = 3 = PROT_READ | PROT_WRITE
  ASM("\xBE\x22\0\0\0"); // mov  esi, $22            ; esi = flags = 0x22 = MAP_ANONYMOUS | MAP_PRIVATE
  ASM("\x31\xFF\x4F");   // xor  edi,edi :: dec edi  ; edi = fd = -1
  ASM("\x31\xED");       // xor  ebp,ebp             ; ebp = off = 0
  ASM("\xCD\x80");       // int  $80                 ; syscall(). puts pointer to obtained memory in eax
  
  //some more setup stuff
  ASM("\x89\xC1");  // mov  ecx, eax ; we want the tape pointer in ecx in order to pass it to write() and read() directly as the buf argument
  ASM("\xB2\1");    // mov   dl, 1   ; dl is low byte of edx, and previously rdx = 3. so now edx = 1. this is the size argument for read() and write()
  //ebx is now 0
  //now for the read/write syscalls, we only need to do this:
  // set eax fully (because read/write might set it to -1)
  // set ebx (fd) to 0 or 1, can be done by only setting the lowest byte since ebx=0 now
  // leave ecx as is (buf = tape pointer)
  // leave edx as is (count = 1)
  // int $80
  
  for (int i = 0; i < prg_len; i++) {
    switch (prg[i]) {
      case '+': ASM("\xFE\1");  break; // inc  BYTE PTR [ecx]  ; ++(*ptr)
      case '-': ASM("\xFE\x9"); break; // dec  BYTE PTR [ecx]  ; --(*ptr)
      case '>': ASM("\x41");    break; // inc  ecx             ; ++ptr
      case '<': ASM("\x49");    break; // dec  ecx             ; --ptr
      case '[': {
        int bal, j, off = 0;
        for (bal = 1, j = i+1; bal != 0 && j < prg_len; j++) {
          bal += prg[j] == '[' ?  1 :
                 prg[j] == ']' ? -1 :
                 0;
          if (bal) off += ins_sizes[(int)prg[j]];
        }
        if (bal != 0) ERR("unbalanced bracket");

        ASM("\x80\x39\x0");                          // cmp BYTE PTR [ecx], 0  ; *ptr == 0 ? 
        ASM("\xF\x84"), fwrite(&off, 4, 1, stdout);  // jz  <offset>           ; if *ptr == 0 jump to matching ']'
      } break;
      case ']': {
        int bal, j, off = -ins_sizes[(int)']'];
        for (bal = 1, j = i-1; bal != 0 && j >= 0; j--) {
          bal += prg[j] == ']' ?  1 :
                 prg[j] == '[' ? -1 :
                 0;
          off -= ins_sizes[(int)prg[j]];
        }
        if (bal != 0) ERR("unbalanced bracket");

        ASM("\x80\x39\x0");                          // cmp BYTE PTR [ecx], 0  ; *ptr == 0 ? 
        ASM("\xF\x85"), fwrite(&off, 4, 1, stdout);  // jnz <offset>           ; if *ptr != 0 jump to matching '['
      } break;
      case '.':
        //execute `write' syscall
        ASM("\x31\xC0\xB0\x4"); // xor  eax,eax :: mov al, 4  ; eax = write Nr. = 4
        ASM("\xB3\1");          // mov  bl, 1                 ; ebx = stdout fd = 1
        ASM("\xCD\x80");        // int  $80                   ; syscall()
        break;
      case ',':
        //execute `read' syscall
        ASM("\x31\xC0\xB0\x3"); // xor  eax,eax :: mov al, 3  ; eax = write Nr. = 3
        ASM("\xB3\0");          // mov  bl, 0                 ; ebx = stdin fd = 0
        ASM("\xCD\x80");        // int  $80                   ; syscall()
        break;
    }
  }

  //and exit(0)
  ASM("\x31\xC0\x40");  // xor  eax,eax :: inc eax  ; eax = exit Nr. = 1
  ASM("\x31\xDB");      // xor  ebx, ebx            ; ebx = exit code = 0
  ASM("\xCD\x80");      // int  $80                 ; syscall()
}
