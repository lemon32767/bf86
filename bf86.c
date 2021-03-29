#include                                         <stdio.h>  
#  define                                       W     fwrite
#    define                                   C      putchar
#      define                               S         stdout
#define  A(x)W                            (x,1,sizeof x-1,S)
static int P[16384],L,I[256],B[256],c,z=64,b,o;int main(){I[
91]=I[93]=(44[I]=I[46]=6+(43[I]=I[45]=++62[I]+(I[60]=++B[91]
)))- --B[93];for(;~(c=getchar())&~255;)I[c]?z+=I[P[L++]=c]:5
;A("\177ELF\1\1\1\x1b[31m >:)\2\0\3\0\1\0\0\0\0\x90\04\0104"
"\0\0\0\xC0\n\0\0!!!??? \0\2\0(\0\1\0\4\0\1!0xBAD! \x80\0\7"
"\0\x80\4\x8t\0\0\0t\0\0\0\4obfsctd\1\0\0\0\0\20\0\0\0\x90"\
"\4\x8\0END");W(&z,4,1,S),W(&z,1,4,S);A("\5\0\0\0\0\x10\0");
for(c=3981;c--;)C(0);A("1\x0C0\260\3001\333\211\331\265x1""\
\322\262\3\276\"\0\0\0\61\377O1\355\315\200\211\301\262\1");
for(;++c<L;)switch(b=P[c]){case'+':case'-':C(-2),C((b-43)*4+
1);break;case'>':case'<':C(65-(b-62)*4);break;case'.':case'\
,':A("1\xC0\xB0"),C(3+(z=b==46)),C(179),C(z),A("\x0CD\x80");
break;case'[':for(o=0,z=c+(b=1);b&&z<L;)if(b+=B[P[z]])o+=I[P
[z++]];case']':if(b)for(z=c-(b=1),o=-9;b&&z>=0;)b-=B[P[z]],o
-=I[P[z--]];A("\2009\0\xF"),C(87+P[c]/2),W(&o,4,1,S);}A("" \
"\x49"        "\300"      "@1\xDB"      "\xCD"     "\200");}
