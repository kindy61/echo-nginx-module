#define DDEBUG 0

#include "ddebug.h"
#include "util.h"
#include "location.h"
#include "handler.h"

static ngx_int_t ngx_http_echo_adjust_subrequest(ngx_http_request_t *sr);

ngx_int_t
ngx_http_echo_exec_echo_location_async(ngx_http_request_t *r,
        ngx_http_echo_ctx_t *ctx, ngx_array_t *computed_args) {
    ngx_int_t                   rc;
    ngx_http_request_t          *sr; /* subrequest object */
    ngx_str_t                   *computed_arg_elts;
    ngx_str_t                   location;
    ngx_str_t                   *url_args;

    computed_arg_elts = computed_args->elts;

    location = computed_arg_elts[0];

    if (location.len == 0) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (computed_args->nelts > 1) {
        url_args = &computed_arg_elts[1];
    } else {
        url_args = NULL;
    }

    DD("location: %s", location.data);
    DD("location args: %s", (char*) (url_args ? url_args->data : (u_char*)"NULL"));

    rc = ngx_http_echo_send_header_if_needed(r, ctx);
    if (r->header_only || rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    rc = ngx_http_subrequest(r, &location, url_args, &sr, NULL, 0);
    if (rc != NGX_OK) {
        return NGX_ERROR;
    }

    rc = ngx_http_echo_adjust_subrequest(sr);
    if (rc != NGX_OK) {
        return rc;
    }

    return NGX_OK;
}

ngx_int_t
ngx_http_echo_exec_echo_location(ngx_http_request_t *r,
        ngx_http_echo_ctx_t *ctx, ngx_array_t *computed_args) {
    ngx_int_t                           rc;
    ngx_http_request_t                  *sr; /* subrequest object */
    ngx_str_t                           *computed_arg_elts;
    ngx_str_t                           location;
    ngx_str_t                           *url_args;
    ngx_http_echo_ctx_t                 *sub_ctx;

    computed_arg_elts = computed_args->elts;

    location = computed_arg_elts[0];

    if (location.len == 0) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (computed_args->nelts > 1) {
        url_args = &computed_arg_elts[1];
    } else {
        url_args = NULL;
    }

    rc = ngx_http_echo_send_header_if_needed(r, ctx);
    if (r->header_only || rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    rc = ngx_http_subrequest(r, &location, url_args, &sr, NULL, 0);
    if (rc != NGX_OK) {
        return NGX_ERROR;
    }

    rc = ngx_http_echo_adjust_subrequest(sr);
    if (rc != NGX_OK) {
        return rc;
    }

    rc = ngx_http_echo_init_ctx(sr, &sub_ctx);
    if (rc != NGX_OK) {
        return NGX_ERROR;
    }

    sub_ctx->cps_ctx = ngx_palloc(sr->pool,
            sizeof(ngx_http_echo_cps_filter_ctx_t));

    if (sub_ctx->cps_ctx == NULL) {
        return NGX_ERROR;
    }

    sub_ctx->cps_ctx->request = r;

    ngx_http_set_ctx(sr, sub_ctx, ngx_http_echo_module);

    ctx->next_handler_cmd++;

    return NGX_OK;
}

static ngx_int_t
ngx_http_echo_adjust_subrequest(ngx_http_request_t *sr) {
    ngx_http_core_main_conf_t  *cmcf;

    /* we do not inherit the parent request's variables */
    cmcf = ngx_http_get_module_main_conf(sr, ngx_http_core_module);
    sr->variables = ngx_pcalloc(sr->pool, cmcf->variables.nelts
                                        * sizeof(ngx_http_variable_value_t));

    if (sr->variables == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    return NGX_OK;
}

