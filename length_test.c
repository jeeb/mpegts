/*
 * This shit compiles with:
 * gcc length_test.c -std=gnu99 -Wall -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I/libav|ffmpeg/install/include
 *
 * We have three possible packet lengths: 188-byte, 192-byte and 204-byte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "mpegts.h"

int main( int argc, char** argv )
{
    FILE *file = NULL;
    uint8_t read_data;
    uint64_t fsize;
    int derp;

    /* Random stuff to calc changes */
    uint64_t last_sync_byte_pos[4];
    uint64_t differences[3];
    uint8_t counter = 0;
    uint64_t how_far = 0;

    /* Load the file and check its size. */
    file = fopen64( "herp.ts", "rb" );

    fseeko64( file, 0L, SEEK_END );
    fsize = ftello64( file );

    if(fsize == 0)
    {
        printf("Error: File size zero, not going further!");
        return 1;
    }

    rewind( file );

    /* Find the first sync byte. */
    do
    {
        derp = fread( &read_data, sizeof( uint8_t ), 1, file );
        if( derp != 1 )
        {
            printf("Error: Errored while reading a byte to find the first sync byte.");
            return 1;
        }
    } while( read_data != SYNC_BYTE );

    how_far = ( ftello64( file ) - 1);
    printf("Found first sync byte at: 0x%lX\n\n", how_far);

    printf("Trying to see if there is another sync byte in 188 bytes: ");

    fseeko64( file, 187L, SEEK_CUR );
    derp = fread( &read_data, sizeof( uint8_t ), 1, file );
    if( derp != 1 )
        return 1;

    if( read_data == SYNC_BYTE )
        printf("yes\n");
    else
        printf("no\n");

    printf("Trying to see if there is another sync byte in 192 bytes: ");

    fseeko64( file, 3L, SEEK_CUR );
    derp = fread( &read_data, sizeof( uint8_t ), 1, file );
    if( derp != 1 )
        return 1;

    if( read_data == SYNC_BYTE )
        printf("yes\n");
    else
        printf("no\n");

    printf("Trying to see if there is another sync byte in 204 bytes: ");

    fseeko64( file, 11L, SEEK_CUR );
    derp = fread( &read_data, sizeof( uint8_t ), 1, file );
    if( derp != 1 )
        return 1;

    if( read_data == SYNC_BYTE )
        printf("yes\n");
    else
        printf("no\n");

    return ( EXIT_SUCCESS );
}
