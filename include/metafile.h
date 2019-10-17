#ifndef METAFILE_H_INCLUDED
#define METAFILE_H_INCLUDED
#include <cutelog.h>
#include <cstr.h>
#include <stdlib.h>

struct _metafile_t;
typedef struct _metafile_t* metafile_t;

metafile_t metafile_new(cutelog_t,/*path to metafile=*/const char *, const char *);
void metafile_free(metafile_t);

int metafile_pull(metafile_t); /* pull all configuration. */

size_t metafile_get_file_count(metafile_t);

cstr_t metafile_get_file_name(metafile_t, size_t);
cstr_t metafile_get_file_path(metafile_t, size_t);

#endif /* METAFILE_H_INCLUDED */
