/**
 * configuration file parser
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.hpp"
#include "utils.hpp"


static void print_config(const config_context_t *config_ctx) {
  LOG(INFO, "Configuration Info:");
  printf("  Storage Path:       %s\n", config_ctx->drive_full_path);
  printf("  Host:               %s\n", config_ctx->host);
  printf("  Port:               %d\n", config_ctx->port);
  printf("  Enable Backup:      %s\n",
         config_ctx->enable_backup ? "true" : "false");
  printf("  Backup Schedule:    %s\n", config_ctx->backup_schedule);
  printf("  Backup Directory:   %s\n", config_ctx->backup_directory);
  printf("  Log Directory:      %s\n", config_ctx->log_directory);
  printf("  Log Rotation Size:  %d MB\n", config_ctx->log_rotation_size);
  printf("  Log Retention Days: %d\n", config_ctx->log_retention_days);
}

void config_cleanup(config_context_t *config_ctx) {
  if (config_ctx == NULL) {
    return;
  }
  free(config_ctx->drive_full_path);
  config_ctx->drive_full_path = NULL;
  free(config_ctx->host);
  config_ctx->host = NULL;
  free(config_ctx->backup_schedule);
  config_ctx->backup_schedule = NULL;
  free(config_ctx->backup_directory);
  config_ctx->backup_directory = NULL;
  free(config_ctx->log_directory);
  config_ctx->log_directory = NULL;
}

int parse_config(const char *filename, config_context_t *config_ctx) {
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
    char *key   = strtok(line, " =");
    char *value = strtok(NULL, " =");

    if (!key || !value) {
      continue;
    }

    // match keys and populate struct
    if (strcmp(key, "Storage") == 0) {
      config_ctx->drive_full_path = strdup(value);
    } else if (strcmp(key, "NetworkHost") == 0) {
      config_ctx->host = strdup(value);
    } else if (strcmp(key, "NetworkPort") == 0) {
      config_ctx->port = atoi(value);
    } else if (strcmp(key, "EnableBackup") == 0) {
      config_ctx->enable_backup = (strcmp(value, "true") == 0);
    } else if (strcmp(key, "BackupSchedule") == 0) {
      config_ctx->backup_schedule = strdup(value);
    } else if (strcmp(key, "BackupDirectory") == 0) {
      config_ctx->backup_directory = strdup(value);
    } else if (strcmp(key, "LogDirectory") == 0) {
      config_ctx->log_directory = strdup(value);
    } else if (strcmp(key, "LogRotationSize") == 0) {
      config_ctx->log_rotation_size = atoi(value);
    } else if (strcmp(key, "LogRetentionDays") == 0) {
      config_ctx->log_retention_days = atoi(value);
    }
  }

  print_config(config_ctx);

  fclose(file);
  return 0;
}
