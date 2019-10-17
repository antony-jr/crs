#include <source_packer.h>
#include <metafile.h>
#include <cutelog.h>
#include <cstr.h>

struct _src_packer_t {
	cutelog_t log;
	metafile_t mfile;
	cstr_t output;
};
#define SRC_PACKER_SZ (sizeof(struct _src_packer_t))

src_packer_t src_packer_new(cutelog_t log, metafile_t m, const char *path){
	src_packer_t r = calloc(1, SRC_PACKER_SZ);
	if(r == NULL)
		return NULL;
	r->mfile = m;
	r->log = log;
	r->output = !path ? cstr_new_ex("./") : cstr_new_ex(path);

	if(r->output == NULL){
		src_packer_free(r);
		return NULL;
	}
	return r;
}

void src_packer_free(src_packer_t obj){
	if(obj == NULL)
		return;

	if(obj->output != NULL)
		cstr_free(obj->output);
	free(obj);
}

int src_packer_run(src_packer_t obj){
	if(obj == NULL)
		return -1;
}
