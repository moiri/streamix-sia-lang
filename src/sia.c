/**
 * Allows to build a graph structure from the sia parser
 *
 * @file    sia.c
 * @author  Simon Maurer
 *
 */

#include <stdlib.h> /* For malloc to add nodes to a linked list */
#include "sia.h"
#include <igraph.h>

/******************************************************************************/
sias_t* sia_add( sia_t* sia, sias_t* list )
{
    sias_t* sias = malloc( sizeof( struct sias_s ) );
    sias->sia = sia;
    sias->next = list;
    return sias;
}

/******************************************************************************/
sia_states_t* sia_add_state( sia_state_t* state, sia_states_t* list )
{
    sia_states_t* states = malloc( sizeof( struct sia_states_s ) );
    states->state = state;
    states->next = list;
    return states;
}

/******************************************************************************/
sia_transitions_t* sia_add_transition( sia_transition_t* transition,
        sia_transitions_t* list )
{
    sia_transitions_t* transitions = malloc(
            sizeof( struct sia_transitions_s ) );
    transitions->transition = transition;
    transitions->next = list;
    return transitions;
}

/******************************************************************************/
void sia_check( sias_t* sias, sia_t** symbols )
{
    sia_t* sia;

    while( sias != NULL ) {
        HASH_FIND( hh_desc, *symbols, sias->sia->name,
                strlen( sias->sia->name ), sia );
        if( sia != NULL )
            printf( "ERROR: redefinition of '%s'\n", sias->sia->name );
        else HASH_ADD( hh_desc, *symbols, name, strlen( sias->sia->name ),
                sias->sia );
        sia = sias->sia;
        sia_check_duplicate( &sia->g, sia->states, &sia->symbols );
        sia_check_undefined( &sia->g, &sia->symbols );

        sias = sias->next;
    }
}

/******************************************************************************/
void sia_check_duplicate( igraph_t* g, sia_states_t* sia,
        sia_state_t** symbols )
{
    int id;
    sia_state_t* state;
    while( sia != NULL ) {
        HASH_FIND_STR( *symbols, sia->state->name, state );
        if( state != NULL ) {
            printf( "ERROR: redefinition of '%s'\n", sia->state->name );
            sia = sia->next;
            continue;
        }
        id = igraph_vcount( g );
        igraph_add_vertices( g, 1, NULL );
        /* igraph_cattribute_VAS_set( g, "label", id, sia->state->name ); */
        sia->state->id = id;
        HASH_ADD_STR( *symbols, name, sia->state );
        sia = sia->next;
    }
}

/******************************************************************************/
void sia_check_undefined( igraph_t* g, sia_state_t** symbols )
{
    int id;
    sia_state_t* state;
    sia_state_t* tmp;
    sia_state_t* state_tgt;
    sia_transitions_t* transitions;
    sia_transition_t* transition;

    HASH_ITER( hh, *symbols, state, tmp ) {
        transitions = state->transitions;
        while( transitions != NULL ) {
            transition = transitions->transition;
            HASH_FIND_STR( *symbols, transition->state, state_tgt );
            if( state_tgt == NULL ) {
                printf( "ERROR: use of undeclared identifier '%s'\n",
                        transition->state );
                transitions = transitions->next;
                continue;
            }
            id = igraph_ecount( g );
            igraph_add_edge( g, state->id, state_tgt->id );
            igraph_cattribute_EAS_set( g, G_SIA_NAME, id, transition->action );
            igraph_cattribute_EAS_set( g, G_SIA_PNAME, id, transition->action );
            igraph_cattribute_EAS_set( g, G_SIA_MODE, id, transition->mode );
            transitions = transitions->next;
        }
    }
}

/******************************************************************************/
sia_t* sia_create( char* name, sia_states_t* states )
{
    sia_t* sia = malloc( sizeof( struct sia_s ) );
    sia->name = name;
    sia->smx_name = NULL;
    sia->states = states;
    sia->symbols = NULL;
    igraph_empty( &sia->g, 0, true );
    return sia;
}

/******************************************************************************/
sia_state_t* sia_create_state( char* name, sia_transitions_t* transitions )
{
    sia_state_t* state = malloc( sizeof( struct sia_state_s ) );
    state->name = name;
    state->transitions = transitions;
    return state;
}

/******************************************************************************/
sia_transition_t* sia_create_transition( char* action , const char* mode,
        char* state )
{
    sia_transition_t* transition = malloc( sizeof( struct sia_transition_s ) );
    transition->action = action;
    transition->mode = mode;
    transition->state = state;
    return transition;
}

/******************************************************************************/
void sia_destroy( sia_t* sia )
{
    sia_destroy_state_symbols( &sia->symbols );
    sia_destroy_states( sia->states );
    igraph_destroy( &sia->g );
    free( sia->name );
    free( sia->smx_name );
    free( sia );
}

/******************************************************************************/
void sia_destroy_states( sia_states_t* states )
{
    sia_states_t* states_tmp;
    sia_transitions_t* transitions;
    sia_transitions_t* transitions_tmp;
    while( states != NULL ) {
        transitions = states->state->transitions;
        while( transitions != NULL ) {
            free( transitions->transition->action );
            free( transitions->transition->state );
            free( transitions->transition );
            transitions_tmp = transitions;
            transitions = transitions->next;
            free( transitions_tmp );
        }
        free( states->state->name );
        free( states->state );
        states_tmp = states;
        states = states->next;
        free( states_tmp );
    }
}

/******************************************************************************/
void sia_destroy_state_symbols( sia_state_t** symbols )
{
    sia_state_t* state;
    sia_state_t* state_tmp;
    HASH_ITER( hh, *symbols, state, state_tmp ) {
        HASH_DEL( *symbols, state );
    }
}

/******************************************************************************/
void sia_write( sia_t* sia, const char* name, const char* out_path,
        const char* format )
{
    FILE* out_file;
    char* out_file_name = NULL;

    out_file_name = malloc( strlen( out_path ) + strlen( name )
            + strlen( format ) + 3 );
    sprintf( out_file_name, "%s/%s.%s", out_path, name, format );
    out_file = fopen( out_file_name, "w" );

    if( strcmp( format, G_FMT_GML ) == 0 ) {
        igraph_write_graph_gml( &sia->g, out_file, NULL, G_GML_HEAD );
    }
    else if( strcmp( format, G_FMT_GRAPHML ) == 0 ) {
        igraph_write_graph_graphml( &sia->g, out_file, 0 );
    }
    else {
        printf( "Unknown format '%s'!\n", format );
    }
    fclose( out_file );
    free( out_file_name );
}

/******************************************************************************/
void sias_destroy( sias_t* sias, sia_t** symbols )
{
    sia_t* sia;
    sia_t* sia_tmp;
    HASH_ITER( hh_desc, *symbols, sia, sia_tmp ) {
        HASH_DELETE( hh_desc, *symbols, sia );
        sia_destroy( sia );
    }
    sias_destroy_list( sias );
}

/******************************************************************************/
void sias_destroy_list( sias_t* sias )
{
    sias_t* sias_tmp;
    while( sias != NULL ) {
        sias_tmp = sias;
        sias = sias->next;
        free( sias_tmp );
    }
}

/******************************************************************************/
void sias_write( sia_t** symbols, const char* out_path,
        const char* format )
{
    sia_t* sia;
    sia_t* tmp;

    HASH_ITER( hh_desc, *symbols, sia, tmp ) {
        sia_write( sia, sia->name, out_path, format );
    }
}
