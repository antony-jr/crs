/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, Antony Jr.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cutelog.h>

/* Emojis */
const char *unicorn_emoji = "🦄";
const char *heavy_check_mark_emoji = "✅";
const char *crossmark_emoji = "❌";
const char *lightbulb_emoji = "💡";
const char *stopsign_emoji = "🛑";
const char *warning_emoji = "🔔";
const char *debug_emoji = "💽";
/* --- */

typedef enum {
    progress,
    success,
    fail,
    info,
    fatal,
    warning,
    debug
} print_type_t;

static const char *prog_series = "\\|/-";
static int prog_series_len = 4;

struct _cutelog_t {
    FILE *output;
    cutelog_mode_t mode;
    int prog_series_pos;
    int prev_print_size;
};
#define CUTELOG_SZ (sizeof(struct _cutelog_t))

static char *get_time() {
    time_t current_time;
    struct tm * time_info;
    static char time_string[9] = {0};
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
    return time_string;
}

static int print(cutelog_t ctx, print_type_t type, const char *fmt, va_list ap) {
    int r = -1;
    char *space = "";
    if(ctx == NULL || !fmt)
        return r;

    if(ctx->mode == cutelog_non_multiline_mode)
        fflush(ctx->output);

    r += fprintf(ctx->output, " [ %s ] ",
                 get_time());
    switch(type) {
    case progress:
        r += fprintf(ctx->output, "(%s \033[35mProgress\033[0m ", unicorn_emoji);
        break;
    case success:
        r += fprintf(ctx->output, "(%s \033[32mSuccess\033[0m", heavy_check_mark_emoji);
        space = "  ";
        break;
    case fail:
        r += fprintf(ctx->output, "(%s \033[31mFailed\033[0m", crossmark_emoji);
        space = "   ";
        break;
    case info:
        r += fprintf(ctx->output, "(%s \033[34mInfo\033[0m", lightbulb_emoji);
        space = "     ";
        break;
    case fatal:
        r += fprintf(ctx->output, "(%s \033[47;31mFatal\033[0m", stopsign_emoji);
        space = "    ";
        break;
    case warning:
        r += fprintf(ctx->output, "(%s \033[33mWarning\033[0m", warning_emoji);
        space = "  ";
        break;
    case debug:
        r += fprintf(ctx->output, "(%s \033[36mDebug\033[0m", debug_emoji);
        space = "    ";
        break;
    default:
        r += fprintf(ctx->output, "(Unknown");
        break;
    }
    if(ctx->mode == cutelog_non_multiline_mode) {
        if(type == progress) {
            r+= fprintf(ctx->output, "%c",
                        prog_series[ctx->prog_series_pos]);
            ctx->prog_series_pos =
                (ctx->prog_series_pos + 1) % prog_series_len;
        }
    }
    r += fprintf(ctx->output, " )%s: ",space);

    r += vfprintf(ctx->output, fmt, ap);

    if(ctx->mode == cutelog_non_multiline_mode) {
        if(ctx->prev_print_size > r) {
            do {
                int fill = ctx->prev_print_size - r + 4;
                while(fill--)
                    putc(' ', ctx->output);
            } while(0);
        }
        r+= fprintf(ctx->output, "\r\r");
    } else
        r+= fprintf(ctx->output, "\n");
    return r + 1;

}

#define CREATE_FUNCTION(name) int cutelog_##name(cutelog_t ctx, const char *fmt, ...){ \
	                         va_list ap; \
	                         int r = 0; \
	                         va_start(ap, NULL); \
	                         r = print(ctx, name , fmt, ap); \
	                         ctx->prev_print_size = r; \
	                         va_end(ap); \
	                         return r; \
	                      }


cutelog_t cutelog_new() {
    return cutelog_new_ex(stdout);
}

cutelog_t cutelog_new_ex(void *out) {
    cutelog_t r = calloc(1, CUTELOG_SZ);
    if(r == NULL)
        return NULL;
    r->output = (!out) ? stdout : out;
    r->mode = cutelog_multiline_mode; /* default */
    return r;
}

void cutelog_free(cutelog_t ctx) {
    if(ctx == NULL)
        return;
    free(ctx);
}


int cutelog_mode(cutelog_t ctx, cutelog_mode_t mode) {
    if(ctx == NULL)
        return -1;

    if(ctx->mode == cutelog_non_multiline_mode &&
            mode == cutelog_multiline_mode) {
        fflush(ctx->output);
        /* avoid overlapping between multiline and non multiline. */
        do { /* code block in c89 version, do not take this away, its not useless. */
            int fill = ctx->prev_print_size + 4;
            while(fill--)
                putc(' ', ctx->output);
            fprintf(ctx->output, "\r\r");
            fflush(ctx->output);
        } while(0);
    } else {
        fflush(ctx->output);
        fprintf(ctx->output, "\r\r");
        fflush(ctx->output);
    }
    ctx->mode = mode;
    return 0;
}

int cutelog_safe_finish(cutelog_t ctx) {
    if(ctx == NULL)
        return -1;

    if(ctx->mode == cutelog_non_multiline_mode)
        fprintf(ctx->output, "\n");
    return 0;
}

CREATE_FUNCTION(progress);
CREATE_FUNCTION(success);
CREATE_FUNCTION(fail);
CREATE_FUNCTION(info);
CREATE_FUNCTION(fatal);
CREATE_FUNCTION(warning);
CREATE_FUNCTION(debug);

