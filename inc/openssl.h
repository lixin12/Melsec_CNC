#ifndef OPENSSL_H
#define OPENSSL_H
#include <stdio.h>

int ssl_client_oneway();
int ssl_client_twoway();
int ssl_server_oneway ();
int ssl_server_twoway ();

#endif
