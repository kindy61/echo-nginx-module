#define DDEBUG 1

#include "ddebug.h"
#include "cps-filter.h"
#include "handler.h"

#include <nginx.h>

ngx_flag_t ngx_http_echo_cps_filter_used = 0;

static ngx_http_output_body_filter_pt ngx_http_next_body_filter;

static ngx_int_t ngx_http_echo_cps_body_filter(ngx_http_request_t *r, ngx_chain_t *in);

ngx_int_t
ngx_http_echo_cps_filter_init (ngx_conf_t *cf) {
    if (ngx_http_echo_cps_filter_used) {
        ngx_http_next_body_filter = ngx_http_top_body_filter;
        ngx_http_top_body_filter  = ngx_http_echo_cps_body_filter;
    }

    return NGX_OK;
}

static ngx_int_t
ngx_http_echo_cps_body_filter(ngx_http_request_t *r, ngx_chain_t *in) {
    ngx_http_request_t                  *orig_r;
    ngx_http_echo_ctx_t                 *ctx;
    ngx_int_t                           rc;
    ngx_flag_t                          last;
    ngx_chain_t                         *cl;

    if (in == NULL || r->header_only) {
        return ngx_http_next_body_filter(r, in);
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_echo_module);

    if (ctx == NULL || ctx->cps_ctx == NULL ||
            ctx->cps_ctx->request == NULL) {
        return ngx_http_next_body_filter(r, in);
    }

    orig_r = ctx->cps_ctx->request;

    DD("Found cps ctx!");

    last = 0;

    for (cl = in; cl; cl = cl->next) {

#if 0
        if (cl->buf->memory) {
            DD("%s", cl->buf->pos);
        } else {
            DD("Not a memory buf! last_buf %d, last_in_chain %d, flush %d, sync %d",
                    cl->buf->last_buf,
                    cl->buf->last_in_chain,
                    cl->buf->flush,
                    cl->buf->sync);
        }
#endif

        if (cl->buf->last_buf || cl->buf->sync) {
            last = 1;
            DD("Found LAST");
            break;
        }
    }

    DD("LAST Found: %d", last);

    rc = ngx_http_next_body_filter(r, in);

    if (rc == NGX_ERROR || !last) {
        return rc;
    }

    ctx->cps_ctx = NULL;

    return ngx_http_echo_handler(orig_r);
}

