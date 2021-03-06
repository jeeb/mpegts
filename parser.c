/*
 * This shit compiles with:
 * gcc parser.c -std=gnu99 -Wall -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
 *
 * We have three possible packet lengths: 188-byte, 192-byte and 204-byte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
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
    rewind( file );

    transport_packet_header tsheader;
    adaptation_field_header afheader;
    pcr pcrdata;
    opcr opcrdata;
    adaptation_field_extension afext;

    uint8_t pcr_salvage[]

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

            printf("Sync byte found at position: 0x%"PRIX64"\n", (uint64_t)( ftello64(file) - 1 ) );

            uint8_t djurpan[2] = { 0 };

            for(int i = 0; i < 2; i++)
            {
                derp = fread( &djurpan[i], sizeof( uint8_t ), 1, file );
                if( derp != 1 )
                {
                    printf( "Derped at reading the first 16 bits after sync byte. i was %d", i );
                    return 1;
                }
            }

            up_to_pid = ( ( djurpan[0] << 8 ) | djurpan[1] );

            #define BIT16 0x8000
            #define BIT15 0x4000
            #define BIT14 0x2000

            tsheader.transport_error_indicator = !!( up_to_pid & BIT16 );
            tsheader.payload_unit_start_indicator = !!( up_to_pid & BIT15 );
            tsheader.transport_error_indicator = !!( up_to_pid & BIT14 );
            tsheader.pid = up_to_pid & 0x1fff;
 
            printf("TEI: %u, PUSI: %u, TP: %u\n", tsheader.transport_error_indicator,
                    tsheader.payload_unit_start_indicator, tsheader.transport_priority );
            printf("Seeming PID: 0x%X\n", tsheader.pid);



            derp = fread( &after_pid, sizeof( uint8_t ), 1, file );
            if( derp!= 1 )
            {
                printf( "Derped at reading the 8 bits after PID.\n" );
                return 1;
            }

            tsheader.transport_scrambling_control = ( after_pid >> 6 ) & 0x3;
            tsheader.adaptation_field_control = ( after_pid >> 4 ) & 0x3;
            tsheader.continuity_counter = after_pid & 0xf;

            printf("TSC: %u, AFC: %u, CC: %u\n", tsheader.transport_scrambling_control,
                   tsheader.adaptation_field_control, tsheader.continuity_counter);

            if( tsheader.pid == 0x111 && tsheader.adaptation_field_control == 3 )
                printf("I am HERE!\n");

            if( tsheader.pid == 0x1FFF )
                printf("We can has null packets!\n");

            /*
             * If AFC == 1, we have only a payload.
             * If AFC == 2, we only have an adaptation field.
             * If AFC == 3, we have an adaptation field as well as a payload.
             */

            if( tsheader.adaptation_field_control == 2 || tsheader.adaptation_field_control == 3 )
            {
                derp = fread( &afheader.length , sizeof( uint8_t ), 1, file );
                if( derp != 1 )
                {
                    printf( "Derped at reading the size of the size of the adaptation field.\n" );
                    return 1;
                }

                if( afheader.length == 0 )
                {
                    printf( "We have an adaptation field, but the size of it is zero. Derp.\n" );
                    return 1;
                }
                else
                {
                    derp = fread( &read_data, sizeof( uint8_t ), 1, file );
                    if( derp != 1 )
                    {
                        printf( "Derped at reading the adaptation field header.\n" );
                        return 1;
                    }

                    afheader.discontinuity_indicator = !!( read_data & ( 1 << 7 ) );
                    afheader.random_access_indicator = !!( read_data & ( 1 << 6 ) );
                    afheader.elementary_stream_priority_indicator = !!( read_data & ( 1 << 5 ) );
                    afheader.pcr_flag = !!( read_data & ( 1 << 4 ) );
                    afheader.opcr_flag = !!( read_data & ( 1 << 3 ) );
                    afheader.splicing_point_flag = !!( read_data & ( 1 << 2 ) );
                    afheader.transport_private_data_flag = !!( read_data & ( 1 << 1 ) );
                    afheader.adaptation_field_extension = !!( read_data & 1 );

                    if( afheader.pcr_flag )
                    {
                        /*derp = fread( &pcr_shit, sizeof( uint32_t ) + sizeof( uint16_t ) )*/
                        /* Herp fucking derp. */
                    }
                }
            }

            if( tsheader.adaptation_field_control == 1 /*|| tsheader.adaptation_field_control == 3*/ )
            {
                printf("No adaptation field, we go on to parse the payload.\n");
            }

            if( counter == 4 ) {
                for( counter = 0; counter < 3; counter++ ) {
                    differences[counter] = last_sync_byte_pos[counter + 1] - last_sync_byte_pos[counter];
                    printf("\n%i. difference is: %"PRIu64"\n", counter + 1, differences[counter]);
                }
                printf("\n");
                counter = 5;
            }

            printf("\n");
        } else
            continue;
    } while( derp == 1 );

    return ( EXIT_SUCCESS );
}

