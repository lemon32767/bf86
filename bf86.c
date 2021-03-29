#include <stdio.h>
#define W fwrite
#define C putchar
#define S stdout
#define A(x)W(x,1,sizeof x-1,S)
static int P[16384],L,I[256],T[256],c,z=64,b,o;
int main() {
  I[91]=I[93]=(44[I]=I[46]=6+(43[I]=I[45]=++62[I]+(I[60]=++T[91])))- --T[93];

  for (;~(c=getchar())&~255;) I[c]?z+=I[P[L++]=c]:5;
  A("\177ELF\1\1\1hello :) \2\0\3\0\1\0\0\0\0\x90\4\0104\0\0\0\xC0\n\0\0!!!??? \0\2\0(\0\1\0\4"
    "\0\1xBASES !\x80\0\7\0\x80\4\x8t\0\0\0t\0\0\0\4obfsctd\1\0\0\0\0\20\0\0\0\x90\4\x8\0END");
  W(&z,4,1,S),W(&z,1,4,S);
  A("\5\0\0\0\0\x10\0");
  for (c=3981;c--;)C(0);

  A("1\300\260\3001\333\211\331\265x1\322\262\3\276\"\0\0\0\61\377O1\355\315\200\211\301\262\1");
  for (;++c<L;) {
    char i[]="1\xC0\xB0.\xB3.\xCD\x80";
    switch (b=P[c]) {
      case'+':case'-': C(-2),C((b-43)*4+1); break;
      case'>':case'<': C(65-(b-62)*4); break;
      case'.':case',': i[3]=3+(i[5]=b==46),A(i); break;
      case'[': for (o=0,z=c+(b=1);b&&z<L;) if(b+=T[P[z]])o+=I[P[z++]];
               A("\2009\0\xF\x84"),W(&o,4,1,S);
               break;
      case']': for (b=1,z=c-1,o=-I[93];b&&z>=0;) b-=T[P[z]],o-=I[P[z--]];
               A("\2009\0\xF\x85"),W(&o,4,1,S);
    }
  }
  A("1\300@1\xDB\xCD\200");
}
