#include <stdlib.h>
#include <stdio.h>
#include <crs_header.h>
#include <cstr.h>
#include <cstr_list.h>
#include <cutelog.h>

#define true 1
#define false 0

struct _crs_header_t {
	cutelog_t log;
	cstr_list_t file_names;
	cstr_list_t file_paths;
	cstr_t path;
};

#define CRS_HEADER_SZ (sizeof(struct _crs_header_t))

crs_header_t crs_header_new(cutelog_t ctx, const char *path){
	crs_header_t r = calloc(1, CRS_HEADER_SZ);
	if(!r)
		return NULL;
	
	r->path = cstr_new_ex(path);
	r->file_paths = cstr_list_new_ex();
	r->file_names = cstr_list_new_ex();
	if(r->path == NULL || 
	   r->file_paths == NULL ||
	   r->file_names == NULL){
		crs_header_free(r);
		return NULL;
	}
	r->log = ctx;
	return r;
}
void crs_header_free(crs_header_t ctx){
	if(ctx == NULL)
		return;
	cstr_list_free(ctx->file_paths, /*free all resource inside it too=*/true);
	cstr_list_free(ctx->file_names, true);
	cstr_free(ctx->path);
	free(ctx);
}

int crs_header_add_file(crs_header_t ctx, const char *filename, const char *filepath){
	if(ctx == NULL || !filename || !filepath)
		return -1;
	cstr_list_append(ctx->file_names, cstr_new_ex(filename), cstr_list_end());
	cstr_list_append(ctx->file_paths, cstr_new_ex(filepath), cstr_list_end());
	return 0;
}

int crs_header_commit(crs_header_t ctx){
	return 0;
}

