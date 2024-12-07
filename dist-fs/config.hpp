#pragma once

typedef struct {
    char *drive_full_path;  // Path to drive, e.g., /dev/disk/by-id/usb-*
    char *host;             // Host address
    int port;               // Port number
    int enable_backup;      // Backup enabled (1 = true, 0 = false)
    char *backup_schedule;  // Backup schedule, e.g., "daily"
    char *backup_directory; // Backup directory
    char *log_directory;    // Log directory
    int log_rotation_size;  // Log rotation size in MB
    int log_retention_days; // Log retention days
} config_info_t;


int parse_config(const char *filename, config_info_t *config_struct);
