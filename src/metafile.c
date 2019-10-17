#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <metafile.h>
#include <cstr_list.h>
#include <cstr.h>
#include <cutelog.h>
#include <json.h>

struct _metafile_t {
	cstr_t path;
	cstr_t prefix;
	cstr_list_t file_names,
		    file_paths;
	cutelog_t log;
	int pulled;
};
#define METAFILE_SZ (sizeof(struct _metafile_t))

static int parse_json_object(json_value *value , metafile_t obj){
	int length = 0 , x =0;
	json_value *v = NULL;
	char *name = NULL;
	int line = 0, col = 0;
	cstr_t buffer;
	if(!value ||
	   value->type != json_object){
		return -1;
	}
	length = value->u.object.length;
	
	for(x = 0; x < length; ++x){
		name = value->u.object.values[x].name; 
		v = value->u.object.values[x].value;
		line = (value->u.object.values[x].value)->line;
		col = (value->u.object.values[x].value)->col;
		
		cutelog_mode(obj->log, cutelog_non_multiline_mode);
		cutelog_progress(obj->log, "Adding: %s", name);

		if(v->type != json_string){
			cutelog_mode(obj->log, cutelog_multiline_mode);
			cutelog_fail(obj->log, "expected a string but something else was given at %d:%d, ignoring.", line,col);
			continue;
		}
		cstr_list_append(obj->file_names, cstr_new_ex(name), cstr_list_end());
		buffer = cstr_new_ex(v->u.string.ptr);
		if(buffer == NULL){
			cutelog_mode(obj->log, cutelog_multiline_mode);
			cutelog_fatal(obj->log, "not enough memory");
			return -1;
		}
		cstr_prepend_ex(buffer, obj->prefix);
		cstr_list_append(obj->file_paths, buffer, cstr_list_end());
		cutelog_mode(obj->log, cutelog_multiline_mode);
		cutelog_success(obj->log, "Added %s", cstr_digest(buffer));
	}
	return 0;
}

static int parse(const char *json_contents , size_t json_contents_len , metafile_t obj){
	json_value *value = NULL;
	int r = 0;
	if(!json_contents || !json_contents_len){
		return (r=-1);
	}
       
	json_settings settings = { 0 };
	settings.settings |= json_enable_comments;
	char error[512];
	value = json_parse_ex(&settings , json_contents , json_contents_len , error);
	if(!value){
		cutelog_mode(obj->log, cutelog_non_multiline_mode);
		cutelog_fail(obj->log, "crs.json:%s" , error);
		return (r=-1);
	}
	if(value->type != json_object){
		cutelog_mode(obj->log, cutelog_non_multiline_mode);	
		cutelog_fatal(obj->log, "crs.json:0:0: expected a json object");
		return (r=-1);
	}
	r = parse_json_object(value , obj); 	
	json_value_free(value);
	return r;
}

metafile_t metafile_new(cutelog_t ctx, const char *path, const char *prefix) {
	metafile_t r = calloc(1, METAFILE_SZ);
	if(r == NULL)
		return NULL;
	if(path == NULL){
		free(r);
		return NULL;
	}
	r->log = ctx;
	r->file_names = cstr_list_new_ex();
	r->file_paths = cstr_list_new_ex();
	r->path = cstr_new_ex(path);
	if(path[strlen(path) - 1] != '/')
		cstr_append_char(r->path, '/');
	if(prefix == NULL)
		r->prefix = cstr_new_ex("./");
	else
		r->prefix = cstr_new_ex(prefix);

	cstr_append(r->path, "crs.json");

	if(r->file_names == NULL ||
	   r->file_paths == NULL ||
	   r->path == NULL ||
	   r->prefix == NULL){
		metafile_free(r);
		return NULL;
	}
	return r;
}

void metafile_free(metafile_t ctx){
	if(ctx == NULL)
		return;
	if(ctx->file_names != NULL)
		cstr_list_free(ctx->file_names, 1);
	if(ctx->file_paths != NULL)
		cstr_list_free(ctx->file_paths, 1);
	if(ctx->path != NULL)
		cstr_free(ctx->path);
	if(ctx->prefix == NULL)
		cstr_free(ctx->prefix);
	free(ctx);
}



int metafile_pull(metafile_t ctx){
	cstr_t buffer = NULL;
	FILE *fp = NULL;
	int c = 0;
	size_t len = 0;
	if(ctx == NULL)
		return -1;
	
	cutelog_mode(ctx->log, cutelog_multiline_mode);
	cutelog_info(ctx->log, "using '%s' as the prefix.", cstr_digest(ctx->prefix));
	
	cutelog_mode(ctx->log, cutelog_non_multiline_mode);

	cutelog_progress(ctx->log, "Opening configuration file...");

	fp = fopen(cstr_digest(ctx->path), "r");
	if(!fp){
		cutelog_mode(ctx->log, cutelog_multiline_mode);
		cutelog_fatal(ctx->log, "cannot open configuration file at %s", cstr_digest(ctx->path));
		return -1;
	}

	cutelog_progress(ctx->log, "Parsing configuration file... ");
	cutelog_progress(ctx->log, "Buffering configuration file to memory...");

	buffer = cstr_new();
	if(buffer == NULL){
		cutelog_mode(ctx->log, cutelog_multiline_mode);
		cutelog_fatal(ctx->log, "not enough memory");
		fclose(fp);
		return -1;
	}

	while((c = getc(fp)) != EOF)
		cstr_append_char(buffer, c);
	fclose(fp);

	cutelog_progress(ctx->log, "Configuration file buffered.");
	cutelog_progress(ctx->log, "Parsing json file...");

	cstr_length(buffer, &len);
	if(parse(cstr_digest(buffer), len, ctx) < 0){
		cutelog_mode(ctx->log, cutelog_multiline_mode);
		cutelog_fatal(ctx->log, "cannot parse configuration file at %s", cstr_digest(ctx->path));
		return -1;
	}
	cutelog_mode(ctx->log, cutelog_multiline_mode);
	cutelog_success(ctx->log, "Configuration file parsed.");
	ctx->pulled = 1;
	return 0;
}

size_t metafile_get_file_count(metafile_t ctx){
	if(ctx == NULL || ctx->pulled == 0)
		return 0;
	return cstr_list_length(ctx->file_names);
}

cstr_t metafile_get_file_name(metafile_t ctx, size_t pos){
	if(ctx == NULL || pos > cstr_list_length(ctx->file_names)){
		return NULL;
	}
	return cstr_list_get(ctx->file_names, pos);
}

cstr_t metafile_get_file_path(metafile_t ctx, size_t pos){
	if(ctx == NULL || pos > cstr_list_length(ctx->file_paths)){
		return NULL;
	}
	return cstr_list_get(ctx->file_paths, pos);

}
