#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define RAMPART_JWT "rampart jwt"

static char *ngx_http_rampart_jwt(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_rampart_jwt_handler(ngx_http_request_t *r);

/**
 * This module provided directive: rampart jwt.
 *
 */
static ngx_command_t ngx_http_rampart_jwt_commands[] = {

    { ngx_string("rampart_jwt"), /* directive */
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1, /* server + location contexts, takes 1 argument */
      ngx_http_rampart_jwt, /* configuration setup function */
      0, /* No offset. Only one context is supported. */
      0, /* No offset when storing the module configuration on struct. */
      NULL},

    ngx_null_command /* command termination */
};

/* The module string. */
static u_char ngx_rampart_jwt[] = RAMPART_JWT;

/* The module context. */
static ngx_http_module_t ngx_http_rampart_jwt_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_rampart_jwt_module = {
    NGX_MODULE_V1,
    &ngx_http_rampart_jwt_module_ctx, /* module context */
    ngx_http_rampart_jwt_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t ngx_http_rampart_jwt_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;

    /* Set the Content-Type header. */
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    // Get directive arguments
    ngx_str_t *value, name;
    u_char *cookie_data;
    size_t cookie_len;

    // Get the arguments
    // TODO where to get cf
    value = cf->args->elts;

    // Get the first argument
    name = value[1];

    // Get contents of the first argument
    cookie_data = name.data;
    cookie_len = name.len;

    // Add Authorization header!!
    set_custom_header_in_headers_out(r, ngx_string("Authorization"), ngx_string(cookie_data));

    /* Allocate a new buffer for sending out the reply. */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    /* Insertion in the buffer chain. */
    out.buf = b;
    out.next = NULL; /* just one buffer */

    b->pos = ngx_rampart_jwt; /* first position in memory of the data */
    b->last = ngx_rampart_jwt + sizeof(ngx_rampart_jwt); /* last position in memory of the data */
    b->memory = 1; /* content is in read-only memory */
    b->last_buf = 1; /* there will be no more buffers in the request */

    /* Sending the headers for the reply. */
    r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
    /* Get the content length of the body. */
    r->headers_out.content_length_n = sizeof(ngx_rampart_jwt);
    ngx_http_send_header(r); /* Send the headers */

    /* Send the body, and return the status code of the output filter chain. */
    return ngx_http_output_filter(r, &out);
}

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_rampart_jwt(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

    /* Install the handler. */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_rampart_jwt_handler;

    return NGX_CONF_OK;
}

static ngx_int_t
set_custom_header_in_headers_out(ngx_http_request_t *r, ngx_str_t *key, ngx_str_t *value) {
    ngx_table_elt_t   *h;

    /*
    All we have to do is just to allocate the header...
    */
    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    /*
    ... setup the header key ...
    */
    h->key = *key;

    /*
    ... and the value.
    */
    h->value = *value;

    /*
    Mark the header as not deleted.
    */
    h->hash = 1;

    return NGX_OK;
}

/**
 * Brute force search for one header with the specified name.
 */
static ngx_table_elt_t *
search_headers_in(ngx_http_request_t *r, u_char *name, size_t len) {
    ngx_list_part_t            *part;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;

    /*
    Get the first part of the list. There is usual only one part.
    */
    part = &r->headers_in.headers.part;
    h = part->elts;

    /*
    Headers list array may consist of more than one part,
    so loop through all of it
    */
    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                /* The last part, search is done. */
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        /*
        Just compare the lengths and then the names case insensitively.
        */
        if (len != h[i].key.len || ngx_strcasecmp(name, h[i].key.data) != 0) {
            /* This header doesn't match. */
            continue;
        }

        /*
        Ta-da, we got one!
        Note, we'v stop the search at the first matched header
        while more then one header may fit.
        */
        return &h[i];
    }

    /*
    No headers was found
    */
    return NULL;
}