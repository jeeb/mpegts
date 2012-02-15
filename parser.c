/*
 * This shit compiles with:
 * gcc main.c -std=gnu99 -Wall -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I/libav|ffmpeg/install/include
 *
 * We have three possible packet lengths: 188-byte, 192-byte and 204-byte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <libavutil/bswap.h>
#include "mpegts.h"

int main( int argc, char** argv ) {
    FILE *file = NULL;
    uint8_t read_data;
    uint64_t fsize;
    int derp;

    /* Random stuff to calc changes */
    uint64_t last_sync_byte_pos[4];
    uint64_t differences[3];
    uint8_t counter = 0;

    /* For parsing Transport Stream Packets */
    uint16_t up_to_pid; /* First 16 bits after sync_byte until PID. */
    uint8_t after_pid; /* 8 bits after PID. */

    /* Load the file and check its size. */
    file = fopen64( "herp.ts", "rb" );

    fseeko64( file, 0L, SEEK_END );
    fsize = ftello64( file );
    fseeko64( file, 0L, SEEK_SET );

    transport_packet_header tsheader;

    do
    {
        derp = fread( &read_data, sizeof( uint8_t ), 1, file );

        if( derp != 1 )
            break;

        if( read_data == SYNC_BYTE )
        {
            if( counter < 4 ) {
                last_sync_byte_pos[counter] = (uint64_t)(ftello64(file) -1);
                counter += 1;
            }

            printf("Sync byte found at position: 0x%lX\n", (uint64_t)ftello64(file) -1);

            derp = fread( &up_to_pid, sizeof( uint16_t ), 1, file );
            if( derp!= 1 )
            {
                printf( "Derped at reading the first 16 bits after sync byte.\n" );
                return 1;
            }

            tsheader.transport_error_indicator = (uint8_t)(up_to_pid >> 15);
            tsheader.payload_unit_start_indicator = (uint8_t)(up_to_pid << 1) >> 15;
            tsheader.transport_priority = (uint8_t)(up_to_pid << 2) >> 15;
            tsheader.pid = av_bswap16( (up_to_pid << 3) >> 3 );
            printf("TEI: %u, PUSI: %u, TP: %u\n", tsheader.transport_error_indicator,
                    tsheader.payload_unit_start_indicator, tsheader.transport_priority );
            printf("Seeming PID: 0x%X\n\n", tsheader.pid);

            derp = fread( &after_pid, sizeof( uint8_t ), 1, file );
            if( derp!= 1 )
            {
                printf( "Derped at reading the 8 bits after PID.\n" );
                return 1;
            }

            if( counter == 4 ) {
                for( counter = 0; counter < 3; counter++ ) {
                    differences[counter] = last_sync_byte_pos[counter + 1] - last_sync_byte_pos[counter];
                    printf("%i. difference is: %lu\n", counter + 1, differences[counter]);
                }
                printf("\n");
                counter = 5;
            }

            tsheader.transport_scrambling_control = after_pid >> 6;
            tsheader.adaptation_field_control = (after_pid << 2) >> 6;
            tsheader.continuity_counter = (after_pid << 4 ) >> 4;

        } else
            continue;
    } while( derp == 1 );

    return ( EXIT_SUCCESS );
}

