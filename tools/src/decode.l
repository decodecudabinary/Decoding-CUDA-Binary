%{
#include <string.h>
#include "decode_common.hpp"
#include "decode.tab.hpp"
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
memory	([-~]\ *)?\|?\[.*\]\|?
decimal_line_number	[\_a-zA-Z0-9]+\ \([0-9]+\)

%%
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
"//"[^\n]* {
	//return(COMMENT);
}
1D|ARRAY_1D|RECT|2D|ARRAY_2D|3D|CUBE|ARRAY_CUBE	{
	yylval.token_.type = type_texture_operand;
	yylval.token_.lexeme = (char *) malloc(strlen(yytext)+1);
	strcpy(yylval.token_.lexeme,yytext);
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
\+QNAN	{
	return(PLUSQNAN);
}
\+SNAN	{
	return(PLUSSNAN);
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
[0-9a-fA-F]{16}\:	{
	yylval.token_.lexeme = (char *) malloc(strlen(yytext)+1);
	strcpy(yylval.token_.lexeme,yytext);
	return(HEXCODE);
}
[A-Z0-9_]*	{
	yylval.token_.lexeme = (char *) malloc(strlen(yytext)+1);
	strcpy(yylval.token_.lexeme,yytext);
	return(OPCODE);
}
;	{
	return(SEMICOLON);
}
[ \t\n\r,]	{
}
\|	{
	return (STRAYPIPE);
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
