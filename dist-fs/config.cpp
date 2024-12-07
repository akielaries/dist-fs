/**
 * configuration file parser
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.hpp"
#include "utils.hpp"


void print_config(const config_info_t *config_struct) {
    LOG(INFO, "Configuration Info:");
    printf("  Storage Path:       %s\n", config_struct->drive_full_path);
    printf("  Host:               %s\n", config_struct->host);
    printf("  Port:               %d\n", config_struct->port);
    printf("  Enable Backup:      %s\n", config_struct->enable_backup ? "true" : "false");
    printf("  Backup Schedule:    %s\n", config_struct->backup_schedule);
    printf("  Backup Directory:   %s\n", config_struct->backup_directory);
    printf("  Log Directory:      %s\n", config_struct->log_directory);
    printf("  Log Rotation Size:  %d MB\n", config_struct->log_rotation_size);
    printf("  Log Retention Days: %d\n", config_struct->log_retention_days);
}

int parse_config(const char *filename, config_info_t *config_struct) {
  LOG(INFO, "Parsing config : {%s}", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening configuration file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        // remove newline character
        line[strcspn(line, "\n")] = '\0';

        // parse key-value pairs
        char *key = strtok(line, " =");
        char *value = strtok(NULL, " =");

        if (!key || !value) {
            continue;
        }

        // match keys and populate struct
        if (strcmp(key, "Storage") == 0) {
            config_struct->drive_full_path = strdup(value);
        } else if (strcmp(key, "Host") == 0) {
            config_struct->host = strdup(value);
        } else if (strcmp(key, "Port") == 0) {
            config_struct->port = atoi(value);
        } else if (strcmp(key, "EnableBackup") == 0) {
            config_struct->enable_backup = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "BackupSchedule") == 0) {
            config_struct->backup_schedule = strdup(value);
        } else if (strcmp(key, "BackupDirectory") == 0) {
            config_struct->backup_directory = strdup(value);
        } else if (strcmp(key, "LogDirectory") == 0) {
            config_struct->log_directory = strdup(value);
        } else if (strcmp(key, "LogRotationSize") == 0) {
            config_struct->log_rotation_size = atoi(value);
        } else if (strcmp(key, "LogRetentionDays") == 0) {
            config_struct->log_retention_days = atoi(value);
        }
    }

    print_config(config_struct);

    fclose(file);
    return 0;
}

