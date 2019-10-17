#ifndef SOURCE_PACKER_H_INCLUDED
#define SOURCE_PACKER_H_INLUCDED
#include <metafile.h>
#include <cutelog.h>

struct _src_packer_t;
typedef struct _src_packer_t* src_packer_t;

src_packer_t src_packer_new(cutelog_t,metafile_t, const char *);
void src_packer_free(src_packer_t);

int src_packer_run(src_packer_t);

#endif /* SOURCE_PACKER_H_INCLUDED */
