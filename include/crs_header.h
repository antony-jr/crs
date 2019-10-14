#ifndef CRS_HEADER_H_INCLUDED
#define CRS_HEADER_H_INCLUDED
#include <cutelog.h>

struct _crs_header_t;
typedef struct _crs_header_t* crs_header_t;

crs_header_t crs_header_new(cutelog_t, const char*);
void crs_header_free(crs_header_t);

int crs_header_add_file(crs_header_t, const char*, const char*);
int crs_header_commit(crs_header_t);

#endif /* CRS_HEADER_H_INCLUDED */
