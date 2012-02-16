#define SYNC_BYTE 0x47

/* Contents of the Transport Stream Packet */

typedef struct
{
    uint8_t  transport_error_indicator:1;
    uint8_t  payload_unit_start_indicator:1;
    uint8_t  transport_priority:1;
    uint16_t pid:13;
    uint8_t  transport_scrambling_control:2;
    uint8_t  adaptation_field_control:2;
    uint8_t  continuity_counter:4;
} transport_packet_header;

typedef struct
{
    uint8_t length;
    /* If length > 0 */
    uint8_t discontinuity_indicator:1;
    uint8_t random_access_indicator:1;
    uint8_t elementary_stream_priority_indicator:1;
    uint8_t pcr_flag:1;
    uint8_t opcr_flag:1;
    uint8_t splicing_point_flag:1;
    uint8_t transport_private_data_flag:1;
    uint8_t extension_flag:1;
    /* If splicing_point_flag == 1 */
    int8_t splice_countdown;
    /* If transport_private_data_flag == 1 */
    uint8_t transport_private_data_length;
} adaptation_field_header;

typedef struct
{
    uint64_t reference_base:33;
    uint8_t  reserved:6;
    uint16_t reference_extension:9;
} pcr;

typedef struct
{
    uint64_t reference_base:33;
    uint8_t  reserved:6;
    uint16_t reference_extension:9;
} opcr;

typedef struct
{
    uint8_t  length;
    uint8_t  ltw_flag:1;
    uint8_t  piecewise_rate_flag:1;
    uint8_t  seamless_splice_flag:1;
    /* If ltw_flag == 1 */
    uint8_t  ltw_valid_flag:1;
    uint16_t ltw_offset:15;
    /* If piecewise_rate_flag == 1 */
    uint8_t  piecewise_reserved:2;
    uint32_t piecewise_rate:22;
    /* If seamless_splice_flag == 1 */
    uint8_t  splice_type:4;
    uint64_t dts_next_au:33;
} adaptation_field_extension;