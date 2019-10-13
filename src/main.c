#include <stdio.h>
#include <crs.h>

static void print_header();
static void print_usage(const char*);

int main(int ac, char **av){
	
	print_header();
	if(ac == 1){
		something();
		print_usage(av[0]);
		return 0;
	}

	return 0;
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
	(void)prog;
	return;
}
