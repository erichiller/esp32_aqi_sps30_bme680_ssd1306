#ifndef SSH_1106_H
#define SSH_1106_H

#include "sps30.h"
#include <NTPClient.h>

void setup_ssh1106();

void update_ssh1106( sps30_measurement *data, String ntpFormattedTime );

void message_ssh1106( char *message, char *result );

#endif