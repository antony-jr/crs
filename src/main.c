#include <stdio.h>
#include <string.h>
#include <cstr.h>
#include <cutelog.h>
#include <metafile.h>

static const char *star_emoji = "🌟";
static const char *heart_emoji = "💖";

static void print_header();
static void print_usage(const char*);
static int write_crs_header(cutelog_t, const char *);

int main(int ac, char **av){
	int ret_code = 0;
	cstr_t metafile_path = NULL,
               output_path = NULL,
               path_prefix = NULL,
	       prog_name = cstr_new_ex(av[0]);
        cutelog_t log = cutelog_new();	
	metafile_t mfile = NULL;	
	print_header();
	if(ac == 1){
		print_usage(cstr_digest(prog_name));
		goto end;
	}
	++av;
	
	while(*av){
		if(!strcmp(*av, "--output") ||
			 !strcmp(*av, "-o")){
			++av;
			if(!*av){
			ret_code = -1;
			print_usage(cstr_digest(prog_name));
			goto end;	
			}
			output_path = cstr_new_ex(*av);	
		}else if(!strcmp(*av, "--path-prefix") ||
			 !strcmp(*av, "-p")){
			++av;
			if(!*av){
			ret_code = -1;
			print_usage(cstr_digest(prog_name));
			goto end;	
			}
			path_prefix = cstr_new_ex(*av);
		}else if(!strcmp(*av, "-h") ||
		         !strcmp(*av, "--help")){
			print_usage(cstr_digest(prog_name));
			goto end;
		}else if(!strcmp(*av, "-v") || !strcmp(*av, "--version")){
			goto end;
		}
		else{
			if(metafile_path == NULL){
				metafile_path = cstr_new_ex(*av);
				++av;
				continue;
			}
			ret_code = -1;
			print_usage(cstr_digest(prog_name));
			goto end;
		}
		++av;
	}

	if((mfile = metafile_new(log, cstr_digest(metafile_path), cstr_digest(path_prefix))) == NULL){
		cutelog_mode(log, cutelog_multiline_mode);
		cutelog_fail(log, "cannot construct metafile context, giving up.");
		ret_code = -1;
		goto end;
	}

	if(metafile_pull(mfile) < 0){
		ret_code = -1;
		goto end;
	}
	
	if(write_crs_header(log, cstr_digest(output_path)) < 0){
		ret_code = -1;
		goto end;
	}

	printf(
	"\nThank you for using crs %s, if you find this project useful then please\n"
	"consider to %s this project at https://github.com/antony-jr/crs.\n",
	heart_emoji,
	star_emoji);

end:
	cstr_free(metafile_path);
        cstr_free(output_path);
        cstr_free(path_prefix);
	cstr_free(prog_name);
        cutelog_free(log);
	metafile_free(mfile);
	if(ret_code < 0){
		printf(
		"\n\033[31mC Resource System failed to generate file(s), If you think this was mistake\033[0m\n"
		"\033[31myou can report a issue at https://github.com/antony-jr/crs\033[0m\n"
		);
	}	
	return ret_code;
}

static void print_header() {
    printf("C Resource System git-commit %s, built on %s\n",
#ifdef GIT_COMMIT_STR
           GIT_COMMIT_STR
#else
           "none"
#endif
           ,
           __TIMESTAMP__
          );
    printf("Copyright (C) %s, Antony Jr.\n", __DATE__ + 7);
    putchar('\n');
    return;

}

static void print_usage(const char *prog) {
	printf(
        "Usage: %s [OPTIONS] [PATH TO METAFILE]\n\n"
        "OPTIONS:\n"
        "    -o,--output        file to write the output.(default=./).\n"
        "    -v,--version       show version.\n"
	"    -h,--help          show this help message.\n"
	"    -p,--prefix-path   set a prefix path when searching for files given in metafile(default=./).\n\n"
        "EXAMPLE: \n"
        "    %s . -o generated_sources/ --prefix-path /tmp/generated_temp_files/ \n"
        "    %s ..\n"
        , prog, prog, prog);

	return;
}

static int write_crs_header(cutelog_t log, const char *path){
	const char *header_contents =
	"#ifndef C_RESOURCE_SYSTEM_H_INCLUDED\n"
	"#define C_RESOURCE_SYSTEM_H_INCLUDED\n"
	"#include <stdlib.h>\n"
	"struct _crs_file_t;\n"
	"typedef struct _crs_file_t* crs_file_t;\n"
	"crs_file_t crs_open(const char*);\n"
	"void crs_close(crs_file_t);\n"
	"int crs_getc(crs_file_t);\n"
	"int crs_eof(crs_file_t);\n"
	"int crs_read(void*, size_t, size_t, crs_file_t);\n"
	"#endif /* C_RESOURCE_SYSTEM_H_INCLUDED */\n"
	;
	FILE *fp = NULL;
	cstr_t p = cstr_new();
	if(p == NULL)
		return -1;
	if(!path){
		cstr_append(p, "./");
	}else{
		cstr_append(p, path);
		if(path[strlen(path) - 1] != '/')
		cstr_append_char(p, '/');
	}
	cstr_append(p, "crs.h");
	if(!(fp = fopen(cstr_digest(p), "w"))){	
		cutelog_mode(log, cutelog_multiline_mode);
		cutelog_fatal(log, "cannot open %s, giving up.", cstr_digest(p));
		cstr_free(p);
		return -1;
	}
	cutelog_mode(log, cutelog_non_multiline_mode);
	cutelog_progress(log, "Writing header... ");
	if(fwrite(header_contents,
		  sizeof *header_contents,
		  strlen(header_contents) - 1, fp) < 0){
		cutelog_mode(log, cutelog_multiline_mode);
		cutelog_fatal(log, "Write to header file failed!");
		cstr_free(p);
		return -1;
	}
	cutelog_mode(log, cutelog_multiline_mode);
	cutelog_success(log, "Wrote header file.");
	cstr_free(p);
	return 0;
}
