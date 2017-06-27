#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <igraph.h>
#include "sia2graph.tab.h"
#include "sia.h"

char* __src_file_name;
extern FILE *zzin;
extern int zzparse( void** );
extern int zzlex_destroy();


int main( int argc, char **argv ) {
    void* sias = NULL;
    const char* out_path = NULL;
    const char* format = NULL;
    FILE* src_smx;
    int c;
    sia_format_t format_e;
    sia_t* symbols;

    while( ( c = getopt( argc, argv, "hvo:f:" ) ) != -1 )
        switch( c ) {
            case 'h':
                printf( "Usage:\n  %s [OPTION...] FILE\n\n", argv[0] );
                printf( "Options:\n" );
                printf( "  -h            This message\n" );
                printf( "  -v            Version\n" );
                printf( "  -o 'path'     Path to store the generated files\n" );
                printf( "  -f 'format'   Format of the graph either 'gml' or 'graphml'\n" );
                return 0;
            case 'v':
                printf( "smxc-v0.0.1\n" );
                return 0;
            case 'o':
                out_path = optarg;
                break;
            case 'f':
                format = optarg;
                break;
            case '?':
                if( ( optopt == 'o' ) || ( optopt == 'f' ) )
                    fprintf ( stderr, "Option -%c requires an argument.\n",
                            optopt );
                else if ( isprint (optopt) )
                    fprintf ( stderr, "Unknown option `-%c'.\n", optopt );
                else
                    fprintf ( stderr, "Unknown option character `\\x%x'.\n",
                            optopt );
                return 1;
            default:
                abort();
        }
    if( argc <= optind ) {
        fprintf( stderr, "Missing argument!\n" );
        return -1;
    }
    __src_file_name = argv[ optind ];

    src_smx = fopen( __src_file_name, "r" );
    // make sure it is valid:
    if( !src_smx ) {
        printf( "Cannot open file '%s'!\n", __src_file_name );
        return -1;
    }
    if( format == NULL ) format = "graphml";
    if( strcmp( format, "gml" ) == 0 ) format_e = FMT_GML;
    else if( strcmp( format, "graphml" ) == 0 ) format_e = FMT_GRAPHML;
    else {
        printf( "Unknown format '%s'!\n", format );
        return -1;
    }
    if( out_path == NULL ) out_path = "./";
    // set flex to read from it instead of defaulting to STDIN    yyin = myfile;
    zzin = src_smx;

    // parse through the input until there is no more:
    do {
        zzparse( &sias );
    } while( !feof( zzin ) );
    fclose( src_smx );

    if( sias == NULL ) return -1;

    sia_check( sias, format_e, out_path, &symbols );

    /* if( zznerrs > 0 ) printf( " Error count: %d\n", zznerrs ); */

    // cleanup
    sia_destroy( sias );
    zzlex_destroy();

    return 0;
}
