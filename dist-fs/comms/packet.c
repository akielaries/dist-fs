#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "../utils.hpp"


int form_packet(dist_fs_ops_e command, uint8_t *payload, uint16_t payload_size) {
  int rc = 0;

  switch (command) {

    case DIST_FS_LIST:
      LOG(INFO, "Form packet for DIST_FS_LIST");
      break;

    case DIST_FS_UPLOAD:
      LOG(INFO, "Form packet for DIST_FS_UPLOAD");
      break;

    case DIST_FS_DOWNLOAD:
      LOG(INFO, "Form packet for DIST_FS_UPLOAD");
      break;

    case DIST_FS_DELETE:
      LOG(INFO, "Form packet for DIST_FS_UPLOAD");
      break;

    default:
      LOG(ERR, "Unknown command {%d}", command);
      break;

  }

  return rc;
}
