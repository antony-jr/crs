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
#include <stdlib.h>
#include <string.h>
#include <cstr.h>

struct _cstr_t {
    char *buffer;
    size_t pos,capacity;
};
#define CSTR_SZ (sizeof(struct _cstr_t))
#define INITIAL_BUFFER_SIZE 1024 /* bytes */

static int grow(cstr_t obj) {
    char *guard = NULL;
    if(obj == NULL)
        return -1;

    if(obj->pos >= obj->capacity) {
        guard = realloc(obj->buffer,
                        sizeof(char) * (obj->capacity + INITIAL_BUFFER_SIZE));
        if(!guard)
            return -1;
        obj->capacity += INITIAL_BUFFER_SIZE;
        memset(guard + obj->pos, 0, sizeof(char)*(obj->capacity - obj->pos));
        obj->buffer = guard;
    }
    return 0;
}

cstr_t cstr_new() {
    cstr_t r = calloc(1, CSTR_SZ);
    if(r == NULL)
        return NULL;
    r->buffer = calloc(INITIAL_BUFFER_SIZE, sizeof *(r->buffer));
    if(!r->buffer) {
        cstr_free(r);
        return NULL;
    }
    r->capacity = INITIAL_BUFFER_SIZE;
    return r;
}

cstr_t cstr_new_ex(const char *s) {
    size_t len = strlen(s) + INITIAL_BUFFER_SIZE;
    cstr_t r = calloc(1, CSTR_SZ);
    if(r == NULL)
        return NULL;
    r->buffer = calloc(len, sizeof *(r->buffer));
    if(!r->buffer) {
        cstr_free(r);
        return NULL;
    }

    while(*s)
        r->buffer[r->pos++] = *s++;
    r->capacity = len;
    return r;
}

void cstr_free(cstr_t obj) {
    if(obj == NULL)
        return;
    if(obj->buffer)
        free(obj->buffer);
    free(obj);
}

int cstr_length(cstr_t obj, void *store) {
    size_t *mem = (size_t*)store;
    if(obj == NULL || !mem)
        return -1;
    *mem = obj->pos;
    return 0;
}

int cstr_prepend_char(cstr_t obj, char c) {
    size_t iter = 0;
    if(grow(obj))
        return -1;
    for(iter = obj->pos; iter > 0; --iter) {
        obj->buffer[iter + 1] = obj->buffer[iter];
    }
    obj->buffer[0] = c;
    ++obj->pos;
    return 0;
}

int cstr_append_char(cstr_t obj, char c) {
    if(grow(obj))
        return -1;
    obj->buffer[obj->pos++] = c;
    return 0;
}

int cstr_prepend(cstr_t obj, const char *s) {
    char *g = NULL;
    cstr_t buf = cstr_new();
    if(buf == NULL)
        return -1;
    if(cstr_append(buf, s)) {
        cstr_free(buf);
        return -1;
    }
    if(cstr_append(buf, obj->buffer)) {
        cstr_free(buf);
        return -1;
    }
    if(!buf->buffer) {
        cstr_free(buf);
        return -1;
    }

    if(obj->buffer)
        free(obj->buffer);
    obj->buffer = buf->buffer;
    obj->pos = buf->pos;
    obj->capacity = buf->capacity;
    free(buf);
}

int cstr_prepend_ex(cstr_t obj, cstr_t other) {
    return cstr_prepend(obj, other->buffer);
}

int cstr_append(cstr_t obj, const char *s) {
    while(*s) {
        if(cstr_append_char(obj, *s))
            return -1;
        ++s;
    }
    return 0;
}

int cstr_append_ex(cstr_t obj, cstr_t other) {
    return cstr_append(obj, other->buffer);
}

const char *cstr_digest(cstr_t obj) {
    return (obj == NULL) ? NULL : obj->buffer;
}
