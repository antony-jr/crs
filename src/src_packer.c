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
#include <stdio.h>
#include <string.h>
#include <src_packer.h>
#include <metafile.h>
#include <cutelog.h>
#include <cstr.h>
#include <char2hex.h>

struct _src_packer_t {
    cutelog_t log;
    metafile_t mfile;
    cstr_t output;
};
#define SRC_PACKER_SZ (sizeof(struct _src_packer_t))

src_packer_t src_packer_new(cutelog_t log, metafile_t m, const char *path) {
    src_packer_t r = calloc(1, SRC_PACKER_SZ);
    if(r == NULL)
        return NULL;
    r->mfile = m;
    r->log = log;
    r->output = !path ? cstr_new_ex("./") : cstr_new_ex(path);

    if(path && path[strlen(path) - 1] != '/')
        cstr_append_char(r->output, '/');

    if(r->output == NULL) {
        src_packer_free(r);
        return NULL;
    }
    cstr_append(r->output, "crs.c");

    return r;
}

void src_packer_free(src_packer_t obj) {
    if(obj == NULL)
        return;

    if(obj->output != NULL)
        cstr_free(obj->output);
    free(obj);
}

int src_packer_run(src_packer_t obj) {
    const char *header_section =
        "/* Generated by C Resource System.\n"
        " * Do not edit here. \n"
        "*/\n"
        "#include <crs.h>\n"
        "#include <stdlib.h>\n"
        "#include <stdio.h>\n"
        "#include <string.h>\n"
        "\n";

    const char *contents_head =
        "\n\n\n\nstruct _crs_file_t {\n"
        "   size_t pos;\n"
        "   const char *data;\n"
        "   size_t length;\n"
        "};\n\n"
        "crs_file_t crs_open(const char *filename){\n"
        "    const char *p = NULL;\n"
        "    size_t len = 0;\n"
        "    crs_file_t r = calloc(1, sizeof(struct _crs_file_t));\n"
        "    r->pos = 0;\n"
        "    r->length = 0;\n"
        "    if(0){\n"
        "    /* This is just a thing to continuously fill with else if tags\n"
        "     * without checking on the syntax. */\n"
        "    }\n"
        ;

    /* Write if tags based on the file names. */

    const char *contents_tail =
        "\n\n    if(p == NULL){\n"
        "        crs_close(r);\n"
        "        return NULL;\n"
        "    }\n"
        "    r->length = len;\n"
        "    r->data = p;\n"
        "    return r;\n"
        "}\n\n"
        "void crs_close(crs_file_t file){\n"
        "    if(file == NULL)\n"
        "        return;\n"
        "    free(file);\n"
        "}\n\n"
        "int crs_getc(crs_file_t file){\n"
        "    if(file == NULL || file->pos == file->length)\n"
        "        return CRS_EOF;\n"
        "    return file->data[file->pos++];\n"
        "}\n\n"
        "int crs_eof(crs_file_t file){\n"
        "    return (file == NULL || file->pos == file->length);\n"
        "}\n\n"
        "size_t crs_read(void *buffer, size_t size, size_t nmem, crs_file_t file){\n"
        "    size_t i = 0, j = 0;\n"
        "    int read = 0;\n"
        "    char *d = NULL;\n"
        "    if(file == NULL || buffer == NULL || size == 0 ||"
        "       nmem == 0 || size >= file->length || nmem * size >= file->length){\n"
        "        return -1;\n"
        "    }\n"
        "    d = buffer;\n"
        "    for(i = 0; i < nmem; ++i){\n"
        "        for(j = 0; j < size; ++j){\n"
        "            *d++ = file->data[file->pos++];\n"
        "            ++read;\n"
        "        }\n"
        "    }\n"
        "    return read;\n"
        "}\n\n"
        "int crs_rewind(crs_file_t file){\n"
        "    if(file == NULL)\n"
        "        return -1;\n"
        "    file->pos = 0;\n"
        "    return 0;\n"
        "}\n\n"
        ;
    FILE *fp = NULL;
    FILE *file_read = NULL;
    size_t iter = 0;
    size_t bytes_read = 0;
    int c = 0;
    size_t failed = 0;
    cstr_t file_name, file_path;
    cstr_t iftag_buffer = cstr_new();
    char size_t_buffer[50] = {0};
    if(obj == NULL)
        return -1;


    cutelog_mode(obj->log, cutelog_non_multiline_mode);
    cutelog_progress(obj->log, "Preparing to write source file... ");

    if(!(fp = fopen(cstr_digest(obj->output), "w"))) {
        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_fatal(obj->log, "cannot open output file '%s' for writing.", cstr_digest(obj->output));
        cstr_free(iftag_buffer);
        return -1;
    }
    cutelog_progress(obj->log, "Writing boilerplate code... ");
    if(fwrite(header_section,
              sizeof *header_section,
              strlen(header_section) - 1, fp) < 0) {
        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_fatal(obj->log, "write failed!");
        fclose(fp);
        cstr_free(iftag_buffer);
        return -1;
    }

    /* We will now loop through the file list and write all
     * variables. */
    iter = 0;
    while(iter < metafile_get_file_count(obj->mfile)) {
        file_name = metafile_get_file_name(obj->mfile, iter);
        file_path = metafile_get_file_path(obj->mfile, iter);
        cutelog_mode(obj->log, cutelog_non_multiline_mode);
        cutelog_progress(obj->log, "Packing %s... ", cstr_digest(file_name));

        cutelog_progress(obj->log, "Opening %s... ", cstr_digest(file_name));

        if(!(file_read = fopen(cstr_digest(file_path), "rb"))) {
            cutelog_mode(obj->log, cutelog_multiline_mode);
            cutelog_fail(obj->log, "cannot open file '%s' for reading, ignoring.", cstr_digest(file_path));
            ++failed;
            ++iter;
            continue;
        }

        /* add the if tag for this file. */
        cstr_append(iftag_buffer, "    else if(!strcmp(\"");
        cstr_append(iftag_buffer, cstr_digest(file_name));
        cstr_append(iftag_buffer, "\", filename)){\n");
        cstr_append(iftag_buffer, "        p = file");
        sprintf(size_t_buffer, "%zu", iter);
        cstr_append(iftag_buffer, size_t_buffer);
        cstr_append(iftag_buffer, "_data;\n");
        cstr_append(iftag_buffer, "        len = file");
        cstr_append(iftag_buffer, size_t_buffer);
        cstr_append(iftag_buffer, "_length;\n");
        cstr_append(iftag_buffer, "    }\n");

        /* start variable declaration. */
        fprintf(fp, "/* START FILE: %s */\n", cstr_digest(file_name));
        fprintf(fp, "static const char *file%zu_data =\n", iter);
        fprintf(fp, "\"");

        bytes_read = 0;
        cutelog_mode(obj->log, cutelog_non_multiline_mode);
        while((c = getc(file_read)) != EOF) {
            fprintf(fp, "%s", char2hex(c));
            ++bytes_read;
            cutelog_progress(obj->log, "Processed %zu bytes... ", bytes_read);
        }
        fclose(file_read);

        fprintf(fp, "\";\n");
        fprintf(fp, "size_t file%zu_length = %zu;\n\n", iter, bytes_read);
        fprintf(fp, "/* END FILE: %s */\n\n\n\n", cstr_digest(file_name));
        /*end variable declaration. */

        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_success(obj->log, "Added %s", cstr_digest(file_name));
        ++iter;
    }


    /* write contents. */
    if(fwrite(contents_head,
              sizeof *contents_head,
              strlen(contents_head) - 1, fp) < 0) {
        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_fatal(obj->log, "write failed!");
        fclose(fp);
        cstr_free(iftag_buffer);
        return -1;
    }

    cstr_length(iftag_buffer, &iter);

    /* write if tags. */
    if(fwrite(cstr_digest(iftag_buffer),
              sizeof(char),
              iter - 1, fp) < 0) {
        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_fatal(obj->log, "write failed!");
        fclose(fp);
        cstr_free(iftag_buffer);
        return -1;
    }
    cstr_free(iftag_buffer);

    if(fwrite(contents_tail,
              sizeof *contents_tail,
              strlen(contents_tail) - 1, fp) < 0) {
        cutelog_mode(obj->log, cutelog_multiline_mode);
        cutelog_fatal(obj->log, "write failed!");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    cutelog_mode(obj->log, cutelog_multiline_mode);
    cutelog_success(obj->log, "All files processed and written.");
    return 0;
}
