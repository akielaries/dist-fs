#include "packet.h"
#include <stdlib.h>
#include <string.h>

int dist_fs_packet_init(dist_fs_packet_t *packet, dist_fs_ops_e command, const uint8_t *payload, uint16_t payload_size) {
    if (!packet) return -1;

    packet->start[0] = (DIST_FS_START_BYTES >> 8) & 0xFF;
    packet->start[1] = DIST_FS_START_BYTES & 0xFF;
    packet->command = command;
    packet->payload_size = payload_size;

    if (payload_size > 0) {
        packet->payload = (uint8_t *)malloc(payload_size);
        if (!packet->payload) return -1;
        memcpy(packet->payload, payload, payload_size);
    } else {
        packet->payload = NULL;
    }

    return 0;
}

int dist_fs_packet_serialize(const dist_fs_packet_t *packet, uint8_t *buffer, size_t buffer_size) {
    if (!packet || !buffer || buffer_size < (DIST_FS_HEADER_SIZE + packet->payload_size + 1)) return -1;

    buffer[DIST_FS_PKT_START_1] = packet->start[0];
    buffer[DIST_FS_PKT_START_2] = packet->start[1];
    buffer[DIST_FS_PKT_COMMAND] = packet->command;
    buffer[DIST_FS_PKT_SIZE_MSB] = (packet->payload_size >> 8) & 0xFF;
    buffer[DIST_FS_PKT_SIZE_LSB] = packet->payload_size & 0xFF;

    if (packet->payload_size > 0) {
        memcpy(&buffer[DIST_FS_PKT_PAYLOAD], packet->payload, packet->payload_size);
    }

    // Calculate and append checksum
    uint8_t checksum = dist_fs_packet_calculate_checksum(buffer, DIST_FS_HEADER_SIZE + packet->payload_size);
    buffer[DIST_FS_HEADER_SIZE + packet->payload_size] = checksum;

    return DIST_FS_HEADER_SIZE + packet->payload_size + 1; // Total size
}

int dist_fs_packet_parse(const uint8_t *buffer, size_t buffer_size, dist_fs_packet_t *packet) {
    if (!buffer || !packet || buffer_size < DIST_FS_HEADER_SIZE + 1) return -1;

    // Verify start bytes
    if (buffer[DIST_FS_PKT_START_1] != ((DIST_FS_START_BYTES >> 8) & 0xFF) ||
        buffer[DIST_FS_PKT_START_2] != (DIST_FS_START_BYTES & 0xFF)) {
        return -1; // Invalid start bytes
    }

    packet->start[0] = buffer[DIST_FS_PKT_START_1];
    packet->start[1] = buffer[DIST_FS_PKT_START_2];
    packet->command = (dist_fs_ops_e)buffer[DIST_FS_PKT_COMMAND];
    packet->payload_size = (buffer[DIST_FS_PKT_SIZE_MSB] << 8) | buffer[DIST_FS_PKT_SIZE_LSB];

    if (packet->payload_size > 0) {
        packet->payload = (uint8_t *)malloc(packet->payload_size);
        if (!packet->payload) return -1;
        memcpy(packet->payload, &buffer[DIST_FS_PKT_PAYLOAD], packet->payload_size);
    } else {
        packet->payload = NULL;
    }

    // Verify checksum
    uint8_t checksum = buffer[DIST_FS_HEADER_SIZE + packet->payload_size];
    if (checksum != dist_fs_packet_calculate_checksum(buffer, DIST_FS_HEADER_SIZE + packet->payload_size)) {
        dist_fs_packet_free(packet);
        return -1; // Invalid checksum
    }

    return 0;
}

uint8_t dist_fs_packet_calculate_checksum(const uint8_t *buffer, size_t size) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= buffer[i];
    }
    return checksum;
}

void dist_fs_packet_free(dist_fs_packet_t *packet) {
    if (packet && packet->payload) {
        free(packet->payload);
        packet->payload = NULL;
    }
}

