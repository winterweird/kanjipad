/* KanjiPad - Japanese handwriting recognition front end
 * Copyright (C) 1997 Owen Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** Refactored and modified by Vegard Itland (2017)
 *
 *  NOTE: This is an original feature I (Vegard) am developing. It's based on
 *  the RESTful API of jisho.org, an online Japanese dictionary. Much thanks to
 *  the creator of the jisho.org website and the Stack Overflow user whose code
 *  example was a tremendous help in getting HTTP GET requests working in C.
 */

#include "jisho_search.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// returns dynamically allocated memory which must be freed.
// Most of the implementation details owe thanks to this StackOverflow post:
// https://stackoverflow.com/a/22135885/4498826
char* jisho_search_keyword(const char* keyword) {
    
    int portno = 80;
    char* host = "www.jisho.org";
    char* message_fmt =
"GET /api/v1/search/words?keyword=%s HTTP/1.0\r\n\
Host: jisho.org\r\n\r\n";

    struct hostent* server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024], response[65535];

    // build the GET request
    sprintf(message, message_fmt, keyword);

    // create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        g_printerr("Error opening socket to jisho.org\n");
        return NULL;
    }

    // lookup ip address
    server = gethostbyname(host);
    if (server == NULL) {
        g_printerr("Error: no such host (%s)\n", host);
        return NULL;
    }

    // fill in structure
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // connect the socket
    if (connect(sockfd, (struct sockaddr*) & serv_addr, sizeof serv_addr) < 0) {
        g_printerr("Error connecting to jisho.org\n");
        return NULL;
    }

    // send the request
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd, message+sent, total-sent);
        if (bytes < 0) {
            g_printerr("Error writing message to socket\n");
            close(sockfd);
            return NULL;
        }
        if (bytes == 0) {
            break;
        }
        sent += bytes;
    } while (sent < total);

    // receive the response
    memset(&response, 0, sizeof response);
    total = sizeof response - 1;
    received = 0;
    do {
        bytes = read(sockfd, response+received, total-received);
        if (bytes < 0) {
            g_printerr("Error reading response from socket\n");
            close(sockfd);
            return NULL;
        }
        if (bytes == 0) {
            break;
        }
        received += bytes;
    } while (received < total);

    close(sockfd);

    if (received == total) {
        g_printerr("Error storing complete response from socket\n");
        return NULL;
    }

    char* ret = NULL;
    for (const char* jsonResponse = response; *jsonResponse != 0; jsonResponse++) {
        if (strncmp(jsonResponse, "\r\n\r\n", 4) == 0) {
            // start at the first part which is not part of the header
            jsonResponse += 4;
            ret = calloc(strlen(jsonResponse) + 1, sizeof *ret);
            if (ret)
                strcpy(ret, jsonResponse);
            else
                g_printerr("Couldn't allocate memory for response from jisho\n");
            break;
        }
    }

    return ret;
}

