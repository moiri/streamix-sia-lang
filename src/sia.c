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
#include <stdbool.h>

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
void sia_check( sias_t* sias, sia_format_t format, const char* out_path,
        sia_t** symbols )
{
    sia_t* sia;
    FILE* out_file;
    char* out_file_name = NULL;
    const char* format_str;
    igraph_i_set_attribute_table( &igraph_cattribute_table );
    igraph_t g;

    switch( format ) {
        case FMT_GML:
            format_str = "gml";
            break;
        case FMT_GRAPHML:
            format_str = "graphml";
            break;
    }

    while( sias != NULL ) {
        HASH_FIND_STR( *symbols, sias->sia->name, sia );
        if( sia != NULL ) {
            printf( "ERROR: redefinition of '%s'\n", sias->sia->name );
            sias = sias->next;
            continue;
        }
        HASH_ADD_STR( *symbols, name, sias->sia );
        sias->sia->symbols = NULL;
        igraph_empty( &g, 0, true );
        sia_check_duplicate( &g, sias->sia->states, &sias->sia->symbols );
        sia_check_undefined( &g, &sias->sia->symbols );

        out_file_name = malloc( strlen( out_path ) + strlen( sias->sia->name )
                + strlen( format_str ) + 3 );
        sprintf( out_file_name, "%s/%s.%s", out_path, sias->sia->name,
                format_str );
        out_file = fopen( out_file_name, "w" );
        switch( format ) {
            case FMT_GML:
                igraph_write_graph_gml( &g, out_file, NULL, "StreamixC" );
                break;
            case FMT_GRAPHML:
                igraph_write_graph_graphml( &g, out_file, 0 );
                break;
        }
        fclose( out_file );
        free( out_file_name );
        igraph_cattribute_remove_e( &g, "name" );
        igraph_cattribute_remove_e( &g, "mode" );
        igraph_cattribute_remove_e( &g, "label" );
        igraph_destroy( &g );
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
        igraph_cattribute_VAS_set( g, "label", id, sia->state->name );
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

    HASH_ITER(hh, *symbols, state, tmp) {
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
            igraph_cattribute_EAS_set( g, "name", id, transition->action );
            igraph_cattribute_EAS_set( g, "mode", id, transition->mode );
            igraph_cattribute_EAS_set( g, "label", id, transition->label );
            transitions = transitions->next;
        }
    }
}

/******************************************************************************/
sia_t* sia_create( char* name, sia_states_t* states )
{
    sia_t* sia = malloc( sizeof( struct sia_s ) );
    sia->name = name;
    sia->states = states;
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
    transition->label = malloc( strlen( action ) + strlen( mode ) + 1 );
    sprintf( transition->label, "%s%s", transition->action,
            transition->mode );
    return transition;
}

/******************************************************************************/
void sia_destroy( sias_t* sias )
{
    sia_state_t* state;
    sia_state_t* state_tmp;
    sias_t* sias_tmp;
    sia_states_t* states;
    sia_states_t* states_tmp;
    sia_transitions_t* transitions;
    sia_transitions_t* transitions_tmp;
    while( sias != NULL ) {
        states = sias->sia->states;
        HASH_ITER(hh, sias->sia->symbols, state, state_tmp) {
            HASH_DEL( sias->sia->symbols, state );
        }
        while( states != NULL ) {
            transitions = states->state->transitions;
            while( transitions != NULL ) {
                free( transitions->transition->action );
                free( transitions->transition->label );
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
        free( sias->sia->name );
        free( sias->sia );
        sias_tmp = sias;
        sias = sias->next;
        free( sias_tmp );
    }
}
