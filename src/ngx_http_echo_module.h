#ifndef NGX_HTTP_ECHO_MODULE_H
#define NGX_HTTP_ECHO_MODULE_H

#include <ngx_core.h>
#include <ngx_http.h>

/* config directive's opcode */
typedef enum {
    echo_opcode_echo,
    echo_opcode_echo_before_body,
    echo_opcode_echo_after_body,
    echo_opcode_echo_client_request_header,
    echo_opcode_echo_client_request_body,
} ngx_http_echo_opcode_t;

/* all the various config directives (or commands) are
 * divided into two categories: "handler commands",
 * and "filter commands". For instance, the "echo"
 * directive is a handler command while
 * "echo_before_body" is a filter one. */
typedef enum {
    echo_handler_cmd,
    echo_filter_cmd
} ngx_http_echo_cmd_category_t;

/* compiled form of a config directive argument's value */
typedef struct {
    /* holds the raw string of the argument value */
    ngx_str_t       raw_value;

    /* fields "lengths" and "values" are set by
     * the function ngx_http_script_compile,
     * iff the argument value indeed contains
     * nginx variables like "$foo" */
    ngx_array_t     *lengths;
    ngx_array_t     *values;
} ngx_http_echo_arg_template_t;

/* represent a config directive (or command) like "echo". */
typedef struct {
    ngx_http_echo_opcode_t      opcode;

    /* each argument is of type echo_arg_template_t: */
    ngx_array_t                 *args;
} ngx_http_echo_cmd_t;

/* location config struct */
typedef struct {
    /* elements of the following arrays are of type
     * ngx_http_echo_cmd_t */
    ngx_array_t     *handler_cmds;
    ngx_array_t     *before_body_cmds;
    ngx_array_t     *after_body_cmds;
} ngx_http_echo_loc_conf_t;

/* context struct in the request handling cycle, holding
 * the current states of the command evaluator */
typedef struct {
    /* index of the next handler command in
     * ngx_http_echo_loc_conf_t's "handler_cmds" array. */
    ngx_int_t       next_handler_cmd;

    /* index of the next before-body filter command in
     * ngx_http_echo_loc_conf_t's "before_body_cmds" array. */
    ngx_int_t       next_before_filter_cmd;

    /* index of the next after-body filter command in
     * ngx_http_echo_loc_conf_t's "after_body_cmds" array. */
    ngx_int_t       next_after_filter_cmd;
} ngx_http_echo_ctx_t;

#endif /* NGX_HTTP_ECHO_MODULE_H */
