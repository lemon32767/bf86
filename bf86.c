#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR(s) (fflush(stdout), fprintf(stderr, "  ERROR: %s\n", s), exit(127))

static char prg[16*1024]; //16kB stripped of non instruction characters should be enough
static int prg_len = 0;

int main(int argc, char** argv) {
  // size in bytes of the assembly generated for each bf instruction. every other character is zero
  const char ins_sizes[256] = {
    ['+'] = 2, ['-'] = 2, ['>'] = 1, ['<'] = 1, ['['] = 9, [']'] = 9, ['.'] = 16, [','] = 15
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
  ASM("\xB8\xC0\0\0\0"); // mov  eax, $C0           ; eax = mmap2 Nr. = 0xC0
  ASM("\x31\xDB");       // xor  ebx, ebx           ; ebx = addr = 0
  ASM("\xB9\0\x78\0\0"); // mov  ecx, $4000         ; ecx = size = 0x7800 = 30 kB ~ 30_000 cells
  ASM("\xBA\3\0\0\0");   // mov  edx, 3             ; edx = prot = 3 = PROT_READ | PROT_WRITE
  ASM("\xBE\x22\0\0\0"); // mov  ESI, $22           ; esi = flags = 0x22 = MAP_ANONYMOUS | MAP_PRIVATE
  ASM("\x31\xFF\x4F");   // xor  EDI,EDI :: dec EDI ; edi = fd = -1
  ASM("\x31\xED");       // xor  EBP,EBP            ; ebp = off = 0
  ASM("\xCD\x80");       // int  $80                ; syscall()

  //now eax = tape pointer 
  
  for (int i = 0; i < prg_len; i++) {
    switch (prg[i]) {
      case '+': ASM("\xFE\0");  break; // inc  BYTE PTR [eax]  ; ++(*ptr)
      case '-': ASM("\xFE\x8"); break; // dec  BYTE PTR [eax]  ; --(*ptr)
      case '>': ASM("\x40");    break; // inc  eax             ; ++ptr
      case '<': ASM("\x48");    break; // dec  eax             ; --ptr
      case '[': {
        int bal, j, off = 0;
        for (bal = 1, j = i+1; bal != 0 && j < prg_len; j++) {
          bal += prg[j] == '[' ?  1 :
                 prg[j] == ']' ? -1 :
                 0;
          if (bal) off += ins_sizes[(int)prg[j]];
        }
        if (bal != 0) ERR("unbalanced bracket");

        ASM("\x80\x38\x0");                          // CMP BYTE PTR [eax], 0  ; *ptr == 0 ? 
        ASM("\xF\x84"), fwrite(&off, 4, 1, stdout);  // JZ  <offset>           ; if *ptr == 0 jump to matching ']'
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

        ASM("\x80\x38\x0");                          // CMP BYTE PTR [eax], 0  ; *ptr == 0 ? 
        ASM("\xF\x85"), fwrite(&off, 4, 1, stdout);  // JNZ <offset>           ; if *ptr != 0 jump to matching '['
      } break;
      case '.':
        //execute `write' syscall
        ASM("\x31\xDB\x43");    // xor  ebx, ebx :: inc ebx    ; ebx = fd = 1 = stdout
        ASM("\x89\xC1");        // mov  ecx, eax               ; ecx = buf = ptr
        ASM("\x31\xD2\x42");    // xor  edx, edx :: inc edx    ; edx = count = 1
        ASM("\x50");            // push eax                    ; save eax
        ASM("\x31\xC0\xB0\x4"); // xor  eax, eax :: mov AL, 4  ; eax = write Nr. = 4
        ASM("\xCD\x80");        // int  $80                    ; syscall()
        ASM("\x58");            // pop  eax                    ; restore eax
        break;
      case ',':
        //execute `read' syscall
        ASM("\x31\xDB");        // xor  ebx, ebx               ; ebx = fd = 0 = stdin
        ASM("\x89\xC1");        // mov  ecx, eax               ; ecx = buf = ptr
        ASM("\x31\xD2\x42");    // xor  edx, edx :: inc edx    ; edx = count = 1
        ASM("\x50");            // push eax                    ; save eax
        ASM("\x31\xC0\xB0\x3"); // xor  eax, eax :: mov AL, 3  ; eax = read Nr. = 3
        ASM("\xCD\x80");        // int  $80                    ; syscall()
        ASM("\x58");            // pop  eax                    ; restore eax
        break;
    }
  }

  //and exit(0)
  ASM("\xB8\x1\0\0\0"); // mov  eax, 1   ; eax = exit Nr. = 1
  ASM("\x31\xDB");      // xor  ebx, ebx ; ebx = 0
  ASM("\xCD\x80");      // int  $80      ; syscall()
}
