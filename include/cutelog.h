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
#ifndef CUTELOG_H_INCLUDED
#define CUTELOG_H_INCLUDED

struct _cutelog_t;
typedef struct _cutelog_t* cutelog_t;

typedef enum {
    cutelog_no_mode,
    cutelog_multiline_mode,
    cutelog_non_multiline_mode
} cutelog_mode_t;

cutelog_t cutelog_new();
cutelog_t cutelog_new_ex(void*);
void cutelog_free(cutelog_t);

int cutelog_mode(cutelog_t, cutelog_mode_t);
int cutelog_safe_finish(cutelog_t);

int cutelog_progress(cutelog_t, const char*, ...);
int cutelog_success(cutelog_t, const char*, ...);
int cutelog_fail(cutelog_t, const char*, ...);
int cutelog_info(cutelog_t, const char*, ...);
int cutelog_fatal(cutelog_t, const char*, ...);
int cutelog_warning(cutelog_t, const char*, ...);
int cutelog_debug(cutelog_t, const char*, ...);


#endif /* CUTELOG_H_INCLUDED */
