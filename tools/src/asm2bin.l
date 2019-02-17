%{
#include <string.h>
#include "common.hpp"
#include "asm2bin.tab.hpp"
%}
%option nounput
%option noyywrap
	extern YYSTYPE yylval;
	extern int isatty();

hex	[-~]?(0x[0-9a-fA-F]+)|(\|0x[0-9a-fA-F]+\|)
bitlist	\{([0-9]*,)*[0-9]*\}
float	\-?[0-9]+(\.[0-9]+)?(e[\+\-][0-9]+)?
pred	!?(P[0-6])|!?[pP][tT]
reg	-?[-~]?\|?(RZ|R[0-9]+)\|?
specialName	SR_[\_\.A-Za-z0-9]+
specialID	SR[0-9]+
const	([-~]\ *)?\|?c\|?\ *\[.*\]\ *\[.*\]\|?
memory	([-~]\ *)?\|?\[[^\[\]]*\]\|?
hexBinaryInst	[0-9a-f]{16}\ \/\/[\ ]
decimal_line_number	[\_a-zA-Z0-9]+\ \([0-9]+\)

%%
{hexBinaryInst}	{}
"//Shared memory usage: "-?0[xX][a-fA-F0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 23;
	return(METADATA_SHAREDMEM);
}
"//Shared memory usage: "-?[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 23;
	return(METADATA_SHAREDMEM);
}
"//Frame Size: "-?0[xX][a-fA-F0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 14;
	return(METADATA_FRAME_SIZE);
}
"//Frame Size: "-?[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 14;
	return(METADATA_FRAME_SIZE);
}
"//Min Stack Size: "-?0[xX][a-fA-F0-9]+[^\n]* {
	yylval.token_.lexeme = yytext + 18;
	return(METADATA_MIN_STACK_SIZE);
}
"//Min Stack Size: "-?[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 18;
	return(METADATA_MIN_STACK_SIZE);
}
"//Max Stack Size: "-?0[xX][a-fA-F0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 18;
	return(METADATA_MAX_STACK_SIZE);
}
"//Max Stack Size: "-?[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 18;
	return(METADATA_MAX_STACK_SIZE);
}
"//Name: "[0-9a-zA-Z_]+[^\n]*	{
	yylval.token_.lexeme = (char*) malloc(strlen(yytext) - 7);
	strcpy(yylval.token_.lexeme, yytext + 8);
	int x = 0;
	while(true) {
		if(yylval.token_.lexeme[x] == 0 || yylval.token_.lexeme[x] == '\n' || yylval.token_.lexeme[x] == '\r') {
			yylval.token_.lexeme[x] = 0;
			break;
		}
		x++;
	}
	return(METADATA_KERNELNAME);
}
"//Arch: sm_"[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 11;
	return(METADATA_ARCH);
}
"//Function count: "[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 18;
	return(METADATA_FUNCTIONCOUNT);
}
"//Function: "[0-9a-zA-Z\_\$]+[^\n]*	{
	yylval.token_.lexeme = (char*) malloc(strlen(yytext) - 11);
	strcpy(yylval.token_.lexeme, yytext + 12);
	int x = 0;
	while(true) {
		if(yylval.token_.lexeme[x] == 0 || yylval.token_.lexeme[x] == '\n' || yylval.token_.lexeme[x] == '\r') {
			yylval.token_.lexeme[x] = 0;
			break;
		}
		x++;
	}
	return(METADATA_FUNCTIONNAME);
}
"//cuobjdump: "[0-9]+[^\n]*	{
	yylval.token_.lexeme = yytext + 13;
	return(METADATA_CUOBJDUMP);
}
"//SCHI: 0x"[0-9a-fA-F]+[^\n]*|"SCHI: 0x"[0-9a-fA-F]+[^\n]*	{
	yylval.token_.lexeme = (char*) malloc(strlen(yytext) - 9);
	strcpy(yylval.token_.lexeme, yytext + 10);
	int x = 0;
	while(true) {
		if(yylval.token_.lexeme[x] == 0 || yylval.token_.lexeme[x] == '\n' || yylval.token_.lexeme[x] == '\r') {
			yylval.token_.lexeme[x] = 0;
			break;
		}
		x++;
	}
	return(INLINE_SCHI_VALUE);
}
"SCHI50:"	{
	return(SCHI50);
}
"//"[^\n]* {
	//return(COMMENT);
}
label[a-zA-Z0-9\_]*\:	{
	yylval.token_.lexeme = (char *) malloc(strlen(yytext)-5);
	yytext[strlen(yytext)-1] = 0;
	strcpy(yylval.token_.lexeme, yytext+5);
	return(LABEL);
}
label[a-zA-Z0-9\_]*	{
	yylval.token_.type = type_texture_operand;
	yylval.token_.lexeme = (char *) malloc(strlen(yytext)-4);
	strcpy(yylval.token_.lexeme, yytext+5);
	return(LABEL_OP);
}
1D|ARRAY_1D|RECT|2D|ARRAY_2D|3D|CUBE|ARRAY_CUBE	{
	yylval.token_.type = type_texture_operand;
	yylval.token_.lexeme = yytext;
	return(TEXOP);
}
R|G|B|A|RG|RA|GA|BA|RGB|RGA|RBA|GBA|RGBA|INVALID5|INVALID6|INVALID7	{
	yylval.token_.type = type_channel;
	yylval.token_.lexeme = yytext;
	return(CHANNEL);
}
SB[0-9] {
	yylval.token_.type = type_sb;
	yylval.token_.lexeme = yytext + 2;
	return(SB_OPERAND);
}
{decimal_line_number} {
	yylval.token_.lexeme = strstr(yytext, "(") + 1;
	return(LINENUMBER);
}
{hex}	{
	yylval.token_.type = type_hex;
	yylval.token_.lexeme = yytext;
	return(HEXVAL);
}
{bitlist}	{
	yylval.token_.type = type_bit_list;
	yylval.token_.lexeme = yytext;
	return(BITLIST);
}
{float}	{
	yylval.token_.type = type_hex;
	yylval.token_.lexeme = yytext;
	return(DECIMAL);
}
\+INF	{
	return(PLUSINF);
}
\-INF	{
	return(NEGINF);
}
\+SNAN {
	return(PLUSSNAN);
}
\+QNAN {
	return(PLUSQNAN);
}
@	{
	return(GUARD);
}
{pred}	{
	yylval.token_.type = type_predicate;
	yylval.token_.lexeme = yytext;
	return(PREDICATE);
}
{reg}	{
	yylval.token_.type = type_register;
	yylval.token_.lexeme = yytext;
	return(REG);
}
{const}	{
	yylval.token_.type = type_const_mem;
	yylval.token_.lexeme = yytext;
	return(CONST);
}
{memory}	{
	yylval.token_.type = type_mem;
	yylval.token_.lexeme = yytext;
	return(MEMORY);
}
{specialName}	{
	yylval.token_.type = type_special_reg;
	yylval.token_.lexeme = yytext;
	return(SPECIALNAME);
}
{specialID}	{
	yylval.token_.type = type_special_reg;
	yylval.token_.lexeme = yytext;
	return(SPECIALID);
}
\.[\?0-9A-Za-z\_]*	{
	yylval.token_.type = type_mod;
	yylval.token_.lexeme = (char *) malloc(strlen(yytext));
	strcpy(yylval.token_.lexeme,yytext+1);
	return(MOD);
}
CC	{
	yylval.token_.type = type_other_operand;
	return(CC);
}
PR	{
	yylval.token_.type = type_other_operand;
	return(PR);
}
MOV	{
	return(MOV);
}
MOV32I	{
	return(MOV32I);
}
LD	{
	return(LD);
}
LDU	{
	return(LDU);
}
LDL	{
	return(LDL);
}
LDS	{
	return(LDS);
}
LDC	{
	return(LDC);
}
ST	{
	return(ST);
}
STL	{
	return(STL);
}
STS	{
	return(STS);
}
LDLK	{
	return(LDLK);
}
LDSLK	{
	return(LDSLK);
}
STUL	{
	return(STUL);
}
STSUL	{
	return(STSUL);
}
FADD	{
	return(FADD);
}
FADD32I	{
	return(FADD32I);
}
FMUL	{
	return(FMUL);
}
FMUL32I	{
	return(FMUL32I);
}
FFMA	{
	return(FFMA);
}
FSET	{
	return(FSET);
}
FSETP	{
	return(FSETP);
}
DSETP	{
	return(DSETP);
}
FCMP	{
	return(FCMP);
}
MUFU	{
	return(MUFU);
}
DADD	{
	return(DADD);
}
DMUL	{
	return(DMUL);
}
DFMA	{
	return(DFMA);
}
IADD	{
	return(IADD);
}
ISUB	{
	return(ISUB);
}
IADD32I	{
	return(IADD32I);
}
IMNMX	{
	return(IMNMX);
}
IMUL	{
	return(IMUL);
}
IMUL32I	{
	return(IMUL32I);
}
IMAD	{
	return(IMAD);
}
ISCADD	{
	return(ISCADD);
}
ISET	{
	return(ISET);
}
ISETP	{
	return(ISETP);
}
ICMP	{
	return(ICMP);
}
I2F	{
	return(I2F);
}
I2I	{
	return(I2I);
}
F2I	{
	return(F2I);
}
F2F	{
	return(F2F);
}
LOP	{
	return(LOP);
}
LOP32I	{
	return(LOP32I);
}
SHL	{
	return(SHL);
}
SHR	{
	return(SHR);
}
BFE	{
	return(BFE);
}
BFI	{
	return(BFI);
}
SEL	{
	return(SEL);
}
SCHI	{
	return(SCHI);
}
SSY	{
	return(SSY);
}
BRA	{
	return(BRA);
}
BRX	{
	return(BRX);
}
PCNT	{
	return(PCNT);
}
CONT	{
	return(CONT);
}
PBK	{
	return(PBK);
}
BRK	{
	return(BRK);
}
CAL	{
	return(CAL);
}
RET	{
	return(RET);
}
EXIT	{
	return(EXIT);
}
NOP	{
	return(NOP);
}
BAR	{
	return(BAR);
}
BPT	{
	return(BPT);
}
B2R	{
	return(B2R);
}
S2R	{
	return(S2R);
}
PSETP	{
	return(PSETP);
}
PSET	{
	return(PSET);
}
FLO	{
	return(FLO);
}
P2R	{
	return(P2R);
}
R2P	{
	return(R2P);
}
TEX	{
	return(TEX);
}
TEXDEPBAR	{
	return(TEXDEPBAR);
}
RRO	{
	return(RRO);
}
PRMT	{
	return(PRMT);
}
VADD	{
	return(VADD);
}
DMNMX	{
	return(DMNMX);
}
FMNMX	{
	return(FMNMX);
}
RED	{
	return(RED);
}
VOTE	{
	return(VOTE);
}
POPC	{
	return(POPC);
}
MEMBAR	{
	return(MEMBAR);
}
STSCUL {
	return(STSCUL);
}
LEPC {
	return(LEPC);
}
CSETP {
	return(CSETP);
}
ISCADD32I {
	return(ISCADD32I);
}
VMNMX {
	return(VMNMX);
}
TLD {
	return(TLD);
}
SHF {
	return(SHF);
}
FCHK {
	return(FCHK);
}
JCAL {
	return(JCAL);
}
SHFL {
	return(SHFL);
}
LDG {
	return(LDG);
}
LD_LDU {
	return(LD_LDU);
}
ATOM {
	return(ATOM);
}
CCTL {
	return(CCTL);
}
XMAD {
	return(XMAD);
}
SYNC {
	return(SYNC);
}
STG {
	return(STG);
}
IADD3 {
	return(IADD3);
}
VABSDIFF {
	return(VABSDIFF);
}
DEPBAR {
	return(DEPBAR);
}
LOP3 {
	return(LOP3);
}
TLDS {
	return(TLDS);
}
TEXS {
	return(TEXS);
}
LEA {
	return(LEA);
}
DSET {
	return(DSET);
}
PHI {
	return(PHI);
}
BINCODE {
	return(BINCODE);
}

[a-fA-F0-9]{16}\:	{
	//garbage
}

[ \t\n\r;,]	{
}
[A-Z0-9]*	{
	printf("\n\nERROR: %s is an unrecognized string here\n\n",yytext);
	yyterminate();
}
.	{
	printf("\n\nERROR: %c is an illegal character here\n\n",yytext[0]);
	yyterminate();
}
%%
#ifdef WINDOWS
	int yylex_destroy() {
		yy_delete_buffer(YY_CURRENT_BUFFER);
		return 0;
	}
#endif
