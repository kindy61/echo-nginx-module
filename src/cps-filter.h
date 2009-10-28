#ifndef ECHO_CPS_FILTER_H
#define ECHO_CPS_FILTER_H

#include "ngx_http_echo_module.h"

extern ngx_flag_t ngx_http_echo_cps_filter_used;

ngx_int_t ngx_http_echo_cps_filter_init (ngx_conf_t *cf);

#endif /* ECHO_CPS_FILTER_H */

