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

#define SYNC_BYTE 0x47

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
    
    /* Contents of the Transport Stream Packet */
    uint8_t transport_error_indicator;
    uint8_t payload_unit_start_indicator;
    uint8_t transport_priority;
    uint16_t pid;
    uint8_t transport_scrambling_control;
    uint8_t adaptation_field_control;
    uint8_t continuity_counter;
    
    /* Load the file and check its size. */
    file = fopen64( "herp.ts", "rb" );
    
    fseeko64( file, 0L, SEEK_END );
    fsize = ftello64( file );
    fseeko64( file, 0L, SEEK_SET );
    
    /* Query packet size with the first N packets. */
    do
    {
    
    } while();
    
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
            
            if( counter == 4 ) {
                for( counter = 0; counter < 3; counter++ ) {
                    differences[counter] = last_sync_byte_pos[counter + 1] - last_sync_byte_pos[counter];
                    printf("%i. difference is: %lu\n", counter + 1, differences[counter]);
                }
                printf("\n");
                counter = 5;
            } 
            
            printf("Sync byte found at position: 0x%lX\n", (uint64_t)ftello64(file) -1);
            if( counter > 1 && counter )
            
            derp = fread( &up_to_pid, sizeof( uint16_t ), 1, file );
            if( derp!= 1 )
            {
                printf( "Derped at reading the first 16 bits after sync byte.\n" );
                return 1;
            }
            
            transport_error_indicator = (uint8_t)(up_to_pid >> 15);
            payload_unit_start_indicator = (uint8_t)(up_to_pid << 1) >> 15;
            transport_priority = (uint8_t)(up_to_pid << 2) >> 15;
            pid = av_bswap16( (up_to_pid << 3) >> 3 );
            printf("TEI: %u, PUSI: %u, TP: %u\n", transport_error_indicator,
                    payload_unit_start_indicator, transport_priority );
            printf("Seeming PID: 0x%X\n\n", pid);
            
            derp = fread( &after_pid, sizeof( uint8_t ), 1, file );
            if( derp!= 1 )
            {
                printf( "Derped at reading the 8 bits after PID.\n" );
                return 1;
            }
            
            transport_scrambling_control = after_pid >> 6;
            adaptation_field_control = (after_pid << 2) >> 6;
            /* continuity_counter = av_bswap8((after_pid << 4 ) >> 4) */
            
        } else
            continue;
    } while( derp == 1 );
    
    return ( EXIT_SUCCESS );
}

