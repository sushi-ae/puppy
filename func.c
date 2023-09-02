#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// xcb
#include <xcb/xcb.h>

#include "puppy.h"

void init(xcb_connection_t *c, xcb_screen_t **s, xcb_window_t *root) {
    if (!(*s)) {
        printf("\033[31mERR\033[0m: failed to get screen information (do you even have one?)\n");
        xcb_disconnect(c);
        exit(1);
    }

    *root = (*s)->root;
    xcb_flush(c);
}

int check_for_wm(xcb_connection_t *c,xcb_screen_t **s,xcb_window_t root){
    root = (*s)->root;

    xcb_intern_atom_cookie_t net_wm_name_cookie = xcb_intern_atom(c,0,
                                                    strlen("_NET_WM_NAME"),"_NET_WM_NAME");

    xcb_intern_atom_cookie_t net_supporting_wm_check_cookie = xcb_intern_atom(c,0,
                                                                strlen("_NET_SUPPORTING_WM_CHECK"),
                                                                "_NET_SUPPORTING_WM_CHECK");

    xcb_intern_atom_reply_t *net_wm_name_reply = xcb_intern_atom_reply(c,net_wm_name_cookie,NULL);

    xcb_intern_atom_reply_t *net_supporting_wm_check_reply = xcb_intern_atom_reply(c,
                                                                net_supporting_wm_check_cookie,NULL);

    xcb_get_property_cookie_t name_cookie = xcb_get_property(c,0,root,net_wm_name_reply->atom,XCB_ATOM_STRING,0,1024);
    xcb_get_property_cookie_t supporting_wm_cookie = xcb_get_property(c,0,root,
                                                        net_supporting_wm_check_reply->atom,
                                                        XCB_ATOM_WINDOW,0,1);

    xcb_get_property_reply_t *name_reply = xcb_get_property_reply(c,name_cookie,NULL);
    xcb_get_property_reply_t *supporting_wm_reply = xcb_get_property_reply(c,supporting_wm_cookie,NULL);

    if (name_reply && xcb_get_property_value_length(name_reply) > 0) {
        free(name_reply);
        free(supporting_wm_reply);
        return 1;
    } 
    else if (supporting_wm_reply && xcb_get_property_value_length(supporting_wm_reply) > 0) {
        free(name_reply);
        free(supporting_wm_reply);
        return 1;
    }
    return 0;
}

