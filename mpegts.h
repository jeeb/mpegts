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
