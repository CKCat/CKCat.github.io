
int main(int argc, char const *argv[])
{

// .text:00000558 F0 B5       PUSH            {R4-R7,LR}              ; Push registers
// .text:0000055A 03 AF       ADD             R7, SP, #0xC            ; Rd = Op1 + Op2
// .text:0000055C 4D F8 04 BD PUSH.W          {R11}                   ; Push registers
// .text:00000560 9C B0       SUB             SP, SP, #0x70           ; Rd = Op1 - Op2 申请栈空间，存放局部变量
// .text:00000562 1A 48       LDR             R0, =(__stack_chk_guard_ptr - 0x568) ; Load from Memory
// .text:00000564 78 44       ADD             R0, PC                  ; __stack_chk_guard_ptr
// .text:00000566 00 68       LDR             R0, [R0]                ; __stack_chk_guard
// .text:00000568 00 68       LDR             R0, [R0]                ; Load from Memory
// .text:0000056A 1B 90       STR             R0, [SP,#0x80+var_14]   ; Store to Memory

// 前面的代码不用管，是编译器自动生成的栈保护代码

// .text:0000056C 01 AC       ADD             R4, SP, #0x80+var_7C    ; Rd = Op1 + Op2
// .text:0000056E 20 46       MOV             R0, R4                  ; Rd = Op2 将局部变量地址作为参数传递，说明局部变量是一个结构提
// .text:00000570 00 F0 B6 FB BL              sub_CE0                 ; Branch with Link
// .text:00000570
// .text:00000574 16 49       LDR             R1, =(aHelloWorld - 0x57E) ; "hello world"
// .text:00000576 20 46       MOV             R0, R4                  ; Rd = Op2 还是前面的局部变量地址
// .text:00000578 0B 22       MOVS            R2, #0xB                ; Rd = Op2 
// .text:0000057A 79 44       ADD             R1, PC                  ; "hello world"
// .text:0000057C 00 F0 C8 FB BL              sub_D10                 ; Branch with Link
// .text:0000057C
// .text:00000580 20 46       MOV             R0, R4                  ; Rd = Op2
// .text:00000582 00 F0 09 FC BL              sub_D98                 ; Branch with Link
// .text:00000582
// .text:00000586 13 48       LDR             R0, =(aMd5sum - 0x58C)  ; "MD5sum: "
// .text:00000588 78 44       ADD             R0, PC                  ; "MD5sum: "
// .text:0000058A 00 F0 82 EC BLX             printf                  ; Branch with Link and Exchange (immediate address)
// .text:0000058A
// .text:0000058E 04 F1 58 05 ADD.W           R5, R4, #0x58 ; 'X'     ; Rd = Op1 + Op2
// .text:00000592 11 4C       LDR             R4, =(a02x - 0x59A)     ; "%02x"
// .text:00000594 00 26       MOVS            R6, #0                  ; Rd = Op2
// .text:00000596 7C 44       ADD             R4, PC                  ; "%02x"
// .text:00000596
// .text:00000598
// .text:00000598             loc_598                                 ; CODE XREF: sub_558+4E↓j
// .text:00000598 10 2E       CMP             R6, #0x10               ; Set cond. codes on Op1 - Op2
// .text:0000059A 05 D0       BEQ             loc_5A8                 ; Branch
// .text:0000059A
// .text:0000059C A9 5D       LDRB            R1, [R5,R6]             ; Load from Memory
// .text:0000059E 20 46       MOV             R0, R4                  ; format
// .text:000005A0 00 F0 76 EC BLX             printf                  ; Branch with Link and Exchange (immediate address)
// .text:000005A0
// .text:000005A4 01 36       ADDS            R6, #1                  ; Rd = Op1 + Op2
// .text:000005A6 F7 E7       B               loc_598                 ; Branch
// .text:000005A6
// .text:000005A8             ; ---------------------------------------------------------------------------
// .text:000005A8
// .text:000005A8             loc_5A8                                 ; CODE XREF: sub_558+42↑j
// .text:000005A8 0A 20       MOVS            R0, #0xA                ; c
// .text:000005AA 00 F0 7A EC BLX             putchar                 ; Branch with Link and Exchange (immediate address)
// .text:000005AA
// .text:000005AE 1B 98       LDR             R0, [SP,#0x80+var_14]   ; Load from Memory
// .text:000005B0 0A 49       LDR             R1, =(__stack_chk_guard_ptr - 0x5B6) ; Load from Memory
// .text:000005B2 79 44       ADD             R1, PC                  ; __stack_chk_guard_ptr
// .text:000005B4 09 68       LDR             R1, [R1]                ; __stack_chk_guard
// .text:000005B6 09 68       LDR             R1, [R1]                ; Load from Memory
// .text:000005B8 81 42       CMP             R1, R0                  ; Set cond. codes on Op1 - Op2
// .text:000005BA 01 BF       ITTTT EQ                                ; If Then
// .text:000005BC 00 20       MOVEQ           R0, #0                  ; Rd = Op2
// .text:000005BE 1C B0       ADDEQ           SP, SP, #0x70 ; 'p'     ; Rd = Op1 + Op2
// .text:000005C0 5D F8 04 BB POPEQ.W         {R11}                   ; Pop registers
// .text:000005C4 F0 BD       POPEQ           {R4-R7,PC}              ; Pop registers
// .text:000005C4
// .text:000005C6 00 F0 74 EC BLX             __stack_chk_fail        ; Branch with Link and Exchange (immediate address)
    return 0;
}


void sub_CE0(void * arg){

// .text:00000CE0 09 49       LDR             R1, =0x10325476         ; Load from Memory
// .text:00000CE2 41 61       STR             R1, [R0,#0x14]          ; Store to Memory

// 结构体偏移 14的位置存放 0x10325476

struct st_unkown{
    int a; 
    int b; 
    int c; 
    int d; 
    int e; 
    int f; // 0x10325476
};

// .text:00000CE4 04 A1       ADR             R1, dword_CF8           ; Load address
// .text:00000CE6 61 F9 CF 0A VLD1.64         {D16-D17}, [R1]         ; Vector Load Single Element
// .text:00000CEA 40 F9 8D 0A VST1.32         {D16-D17}, [R0]!        ; Vector Store Single Element
struct st_unkown{
    int a; // 0
    int b; // 0
    int c; // 0x67452301
    int d; // 0xEFCDAB89
    int e; 
    int f; // 0x10325476
};
// .text:00000CEE 07 49       LDR             R1, =0x98BADCFE         ; Load from Memory
// .text:00000CF0 01 60       STR             R1, [R0]                ; Store to Memory
struct st_unkown{
    int a; // 0
    int b; // 0
    int c; // 0x67452301
    int d; // 0xEFCDAB89
    int e; // 0x98BADCFE
    int f; // 0x10325476
};
// 目前只能知道是一个结构体的这些字段
// .text:00000CF2 70 47       BX              LR                      ; Branch to/from Thumb mode
// .text:00000CF2
// .text:00000CF2             ; End of function sub_CE0
// .text:00000CF2
// .text:00000CF2             ; ---------------------------------------------------------------------------
// .text:00000CF4 00 BF 00 BF DCD 0xBF00BF00
// .text:00000CF8 00 00 00 00+dword_CF8 DCD 0, 0                      ; DATA XREF: sub_CE0+4↑o
// .text:00000D00 01 23 45 67+DCD 0x67452301, 0xEFCDAB89
// .text:00000D08 76 54 32 10 dword_D08 DCD 0x10325476                ; DATA XREF: sub_CE0↑r
// .text:00000D0C FE DC BA 98 dword_D0C DCD 0x98BADCFE                ; DATA XREF: sub_CE0+E↑r
// .text:00000D0C             ; } // starts at CE0
}

void sub_D10(void * arg, char* str, int len){
// .text:00000D10 F0 B5       PUSH            {R4-R7,LR}              ; Push registers
// .text:00000D12 03 AF       ADD             R7, SP, #0xC            ; Rd = Op1 + Op2
// .text:00000D14 2D E9 00 0B PUSH.W          {R8,R9,R11}             ; Push registers
// .text:00000D18 92 B0       SUB             SP, SP, #0x48           ; Rd = Op1 - Op2
// .text:00000D1A 06 46       MOV             R6, R0                  ; Rd = Op2
// .text:00000D1C 1C 48       LDR             R0, =(__stack_chk_guard_ptr - 0xD26) ; Load from Memory
// .text:00000D1E 14 46       MOV             R4, R2                  ; Rd = Op2
// .text:00000D20 89 46       MOV             R9, R1                  ; Rd = Op2
// .text:00000D22 78 44       ADD             R0, PC                  ; __stack_chk_guard_ptr
// .text:00000D24 63 0F       LSRS            R3, R4, #29           ; Logical Shift Right
// .text:00000D26 06 F1 08 08 ADD.W           R8, R6, #8              ; Rd = Op1 + Op2
// .text:00000D2A 00 68       LDR             R0, [R0]                ; __stack_chk_guard
// .text:00000D2C 00 68       LDR             R0, [R0]                ; Load from Memory
// .text:00000D2E 11 90       STR             R0, [SP,#0x60+var_1C]   ; Store to Memory
// 前面需要关注的是 R0 R1 R2 三个寄存存在什么位置，由于流水线优化，执行顺序被打乱了

// .text:00000D30 D6 E9 00 01 LDRD.W          R0, R1, [R6]            ; Load pair of registers
// .text:00000D34 10 EB C4 05 ADDS.W          R5, R0, R4,LSL#3        ; Rd = Op1 + Op2
// .text:00000D38 00 EB C2 02 ADD.W           R2, R0, R2,LSL#3        ; Rd = Op1 + Op2
// .text:00000D3C 59 41       ADCS            R1, R3                  ; Rd = Op1 + Op2 + C
// .text:00000D3E C0 F3 C5 00 UBFX.W          R0, R0, #3, #6          ; Unsigned Bit Field Extract
// .text:00000D42 C6 E9 00 21 STRD.W          R2, R1, [R6]            ; Store pair of registers
// .text:00000D46 01 AD       ADD             R5, SP, #0x60+var_5C    ; Rd = Op1 + Op2
// .text:00000D46
// .text:00000D48
// .text:00000D48             loc_D48                                 ; CODE XREF: sub_D10+48↓j
// .text:00000D48                                                     ; sub_D10+64↓j
// .text:00000D48 AC B1       CBZ             R4, loc_D76             ; Compare and Branch on Zero
// .text:00000D48
// .text:00000D4A 31 18       ADDS            R1, R6, R0              ; Rd = Op1 + Op2
// .text:00000D4C 19 F8 01 2B LDRB.W          R2, [R9],#1             ; Load from Memory
// .text:00000D50 01 3C       SUBS            R4, #1                  ; Rd = Op1 - Op2
// .text:00000D52 01 30       ADDS            R0, #1                  ; Rd = Op1 + Op2
// .text:00000D54 40 28       CMP             R0, #0x40 ; '@'         ; Set cond. codes on Op1 - Op2
// .text:00000D56 0A 76       STRB            R2, [R1,#0x18]          ; Store to Memory
// .text:00000D58 F6 D1       BNE             loc_D48                 ; Branch
// .text:00000D58
// .text:00000D5A 00 20       MOVS            R0, #0                  ; Rd = Op2
// .text:00000D5A
// .text:00000D5C
// .text:00000D5C             loc_D5C                                 ; CODE XREF: sub_D10+58↓j
// .text:00000D5C 40 28       CMP             R0, #0x40 ; '@'         ; Set cond. codes on Op1 - Op2
// .text:00000D5E 04 D0       BEQ             loc_D6A                 ; Branch
// .text:00000D5E
// .text:00000D60 31 18       ADDS            R1, R6, R0              ; Rd = Op1 + Op2
// .text:00000D62 89 69       LDR             R1, [R1,#0x18]          ; Load from Memory
// .text:00000D64 29 50       STR             R1, [R5,R0]             ; Store to Memory
// .text:00000D66 04 30       ADDS            R0, #4                  ; Rd = Op1 + Op2
// .text:00000D68 F8 E7       B               loc_D5C                 ; Branch
// .text:00000D68
// .text:00000D6A             ; ---------------------------------------------------------------------------
// .text:00000D6A
// .text:00000D6A             loc_D6A                                 ; CODE XREF: sub_D10+4E↑j
// .text:00000D6A 40 46       MOV             R0, R8                  ; Rd = Op2
// .text:00000D6C 29 46       MOV             R1, R5                  ; Rd = Op2
// .text:00000D6E FF F7 37 FC BL              sub_5E0                 ; Branch with Link
// .text:00000D6E
// .text:00000D72 00 20       MOVS            R0, #0                  ; Rd = Op2
// .text:00000D74 E8 E7       B               loc_D48                 ; Branch
// .text:00000D74
// .text:00000D76             ; ---------------------------------------------------------------------------
// .text:00000D76
// .text:00000D76             loc_D76                                 ; CODE XREF: sub_D10:loc_D48↑j
// .text:00000D76 11 98       LDR             R0, [SP,#0x60+var_1C]   ; Load from Memory
// .text:00000D78 06 49       LDR             R1, =(__stack_chk_guard_ptr - 0xD7E) ; Load from Memory
// .text:00000D7A 79 44       ADD             R1, PC                  ; __stack_chk_guard_ptr
// .text:00000D7C 09 68       LDR             R1, [R1]                ; __stack_chk_guard
// .text:00000D7E 09 68       LDR             R1, [R1]                ; Load from Memory
// .text:00000D80 81 42       CMP             R1, R0                  ; Set cond. codes on Op1 - Op2
// .text:00000D82 02 BF       ITTT EQ                                 ; If Then
// .text:00000D84 12 B0       ADDEQ           SP, SP, #0x48 ; 'H'     ; Rd = Op1 + Op2
// .text:00000D86 BD E8 00 0B POPEQ.W         {R8,R9,R11}             ; Pop registers
// .text:00000D8A F0 BD       POPEQ           {R4-R7,PC}              ; Pop registers
// .text:00000D8A
// .text:00000D8C 00 F0 90 E8 BLX             __stack_chk_fail        ; Branch with Link and Exchange (immediate address)
}

void sub_D98(void * arg){
    
}