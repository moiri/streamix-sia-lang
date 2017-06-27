/* 
 * The scanner for sia description language
 *
 * @file    sia2graph.lex
 * @author  Simon Maurer
 *
 * */

%{
    #include <stdio.h>
    #include "sia2graph.tab.h"  // to get the token types that we return
    #define ZZ_DECL extern int zzlex()
    void zzerror ( void** sias, const char* s )
    {
        printf( "%d: %s\n", zzlineno, s );
    }
%}
%option noinput
%option nounput
%option yylineno
%option prefix="zz"
%option noyywrap

%x comment
%%
    /* skip whitespaces and CR */
[ \t]           ;
\n              ++zzlloc.last_line;

    /* ignore comments */
"/*"         BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++zzlloc.last_line;
<comment>"*"+"/"        BEGIN(INITIAL);

"//".*          { /* DO NOTHING */ }

    /* keywords */
sia             return KW_SIA;

    /* identifiers */
[a-zA-Z_$][a-zA-Z_$0-9]* {
                zzlval.sval = strdup( zztext );
                return IDENTIFIER;
}

    /* operators */
[.|!?;:{}]     return *zztext;

    /* anything else is an error */
.               zzerror( NULL, "invalid character" );
%%

