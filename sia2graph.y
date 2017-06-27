/* 
 * The grammar of the sia description language
 *
 * @file    sia2graph.y
 * @author  Simon Maurer
 *
 * */

%{
/* Prologue */
    #include "sia.h"
    extern int zzlex();
    extern int zzerror( void*, const char* );
%}

/* Bison declarations */
%parse-param { void** sias }
%define parse.error verbose
%define parse.lac full
%define api.prefix {zz}
%locations
%union {
    int ival;
    char *sval;
    struct sia_s* rval;
    struct sias_s* rsval;
    struct sia_state_s* vval;
    struct sia_states_s* vsval;
    struct sia_transition_s* tval;
    struct sia_transitions_s* tsval;
};

%token KW_SIA
%token <sval> IDENTIFIER
%type <rval> sia
%type <rsval> sias
%type <vval> state
%type <vsval> states
%type <tval> transition
%type <tsval> transition_opt
%type <sval> action_mode

%start program

%%
/* Grammar rules */
/* start of the grammer */

program:
    sias { *sias = $1; }

sias:
    %empty { $$ = ( sias_t* )0; }
|   sia sias { $$ = sia_add( $1, $2 ); }

sia:
    KW_SIA IDENTIFIER '{' state states '}' {
        $$ = sia_create( $2, sia_add_state( $4, $5 ) );
    }
;

states:
    %empty { $$ = ( sia_states_t* )0; }
|   state states { $$ = sia_add_state( $1, $2 ); }
;

state:
    IDENTIFIER ':' transition transition_opt {
        $$ = sia_create_state( $1, sia_add_transition( $3, $4 ) );
    }
;

transition:
    IDENTIFIER action_mode '.' IDENTIFIER {
        $$ = sia_create_transition( $1, $2, $4 );
    }
;

transition_opt:
    %empty { $$ = ( sia_transitions_t* )0; }
|   '|' transition transition_opt { $$ = sia_add_transition( $2, $3 ); }
;

action_mode:
    '?' { $$ = "?"; }
|   '!' { $$ = "!"; }
|   ';' { $$ = ";"; }
;

%%
/* Epilogue */
