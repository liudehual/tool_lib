/*  
 *  
 * int to float for X86  
 *  
 * By Carl. 09-26-2006  
 *  
 */  
#include <stdio.h>  
#include <stdlib.h>  
#include <memory.h>  
#include <math.h>  
#include <string.h>  

#define BITS(a, pos) (((a>>pos)&0x01))  
#define SETBIT(pos)  (1<<pos)  

#pragma pack(1)  
typedef struct Float_TAG  
{  
unsigned short base2;  
unsigned char base1:7;  
unsigned char e2:1;   
unsigned char e1:7;  
unsigned char sig:1;   

}Float, *LPFloat;  
#pragma pack()  


#define MAX_SIZE_OF_BUFFER 32  
#if (MAX_SIZE_OF_BUFFER==32)  
#define MAX_BASE_LENGTH  23  
#define MAX_E_LENGTH     8  

#endif  
char* Hex2Str(unsigned int data, int bits, int* length_sufix)  
{  
static char buffer[MAX_SIZE_OF_BUFFER];  
int i, j , k;  

for(i = 0; i < bits; i++)  
{  
if(BITS(data, i))  
{  
buffer[bits-i-1] = '1';  
}  
else  
{  
buffer[bits-i-1] = '0';  
}  
}  

for(i = 0; i < bits; i++)  
{  
if(buffer != '0')  
break;  
}  
k = i;  
for(j = 0; j < bits; j++, i++)  
{  
        if(j < (bits - k))  
{  
buffer[j] = buffer;  
}  
else  
{  
buffer[j] = 0;  
}  
}  

*length_sufix = strlen(buffer);  

return buffer;  
}  

Float* UInt2Float(int value)  
{  
static char base[MAX_SIZE_OF_BUFFER], *temp = NULL;;  
int length_base,  i;  

static Float  flt;  
unsigned long temp_value, data_length = 0;  
int flag = 0;  

if(value < 0)  
{  

value -=1;  

value  = ~value;  

flag   = 1;  
}  

temp = Hex2Str((unsigned int)value, MAX_SIZE_OF_BUFFER, &length_base);  
memcpy(base, temp, length_base);  

if(value != 0)  
data_length = length_base + 127 - 1;  


temp_value = 0;  

if(length_base > MAX_BASE_LENGTH)  
length_base = MAX_BASE_LENGTH;  


for(i = 1; i < length_base; i++)  
{  
switch(base)  
{  
default:  
break;  
case '1':  
temp_value += SETBIT((MAX_BASE_LENGTH-i));  
break;  
case '0':  
break;  
}  
}  

flt.sig = flag;  

flt.e1 = (data_length>>1)&0x7F;  
flt.e2 = data_length&0x01;  
flt.base1 = (temp_value>>16)&0x7F;  
flt.base2 = ((unsigned short)temp_value&0xFF);  
return &flt;  
}  

int main(int argc, char* argv[])  
{  

for(int i = 0; i < 150; i++)  
{  
LPFloat pv = UInt2Float(-i);  
float uiv = 0;  
memcpy(&uiv, pv, 4);  
printf("%f\n", uiv);  

       pv = UInt2Float(i);  
uiv = 0;  
memcpy(&uiv, pv, 4);  
printf("%f\n", uiv);  
}  
return 0;  
}