//binary stdin data -> C string literal in stdout
#include <ctype.h>
#include <stdio.h>
int main() {
  int c;
  _Bool was_escape = 0;
  putchar('"');
  while ((c = getchar()) != EOF) {
    if (isprint(c)) {
      switch (c) {
        //need to be escaped
#define X(i,c) case i: printf("\\%c",c);break;
      X('\a','a') X('\b','b') X('\f','f') X('\n','n') X('\r','r') X('\t', 't')
      X('\v','v') X('\\','\\') X('\"','\"') X('?','?')
#undef X
      default:
        if (was_escape && isxdigit(c)) printf("\"\"");
        putchar(c);
      }
      was_escape = 0;
    } else {
      if (c < 8) printf("\\%d",c);
      else printf("\\x%X", c);
      was_escape = 1;
    }
  }
  putchar('"');
}
