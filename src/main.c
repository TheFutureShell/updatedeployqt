#include <stdio.h>
#include <string.h>
#include <config_manager.h>
#include <args_parser.h>
#include <logger.h>

static void print_header();
static void print_help();
static void print_conclusion(int);

static char *get_config_file(const char *);

int main(int argc, char **argv) {
	int r = 0;
	int arg_parse_ret = 0;
	char *rbuf = NULL;
	config_manager_t *cmanager = NULL;
	args_parser_t *ap = args_parser_create(argc,argv,
			                       print_header , print_help);
	
	arg_parse_ret = args_parser_run(ap);
	if(arg_parse_ret == ARGS_PARSER_INVALID_OBJECT ||
	   arg_parse_ret == ARGS_PARSER_FATAL_ERROR){
		r = -1;
		goto cleanup;
	}

	if(arg_parse_ret == ARGS_PARSER_NO_ARGS ||
	   arg_parse_ret == ARGS_PARSER_CLEAN_EXIT){
		r = 0;
		goto cleanup;
	}

	if(args_parser_is_quiet(ap)){
		freopen(NULL , "r" , stdout);
	}

	const char *config = args_parser_get_config_file_path(ap);
	const char *deploy_dir = args_parser_get_deploy_dir_path(ap);

	cmanager = config_manager_create(!config ? "./updatedeployqt.json" : (rbuf = get_config_file(config)));
	if(!cmanager){
		r = -1;
		goto cleanup;
	}

	if(config_manager_run(cmanager) < 0){
		r = -1;
		goto cleanup;
	}

cleanup:
	print_conclusion(r);
	args_parser_destroy(ap);
	config_manager_destroy(cmanager);
	
	if(rbuf){
		free(rbuf);
	}
	return r;
}

static void print_conclusion(int result){
	switch(result){
		case 0: /* no-op */
			break;
		case 1:
			printl(info , "successfully deployed updater");
			break;
		default:
			printl(fatal , "deploy failed , exiting with errors");
			break;
	}
	return;
}

/*
 * @desc    : this function simply constructs the path to configuration file with the
 *            given path.
 * @param   : const char * which is assumed to be the path of the configuration file.
 * @returns : char * which is the path to configuration file.
 *
 * @note    : you have to free the output of this function.
*/
static char *get_config_file(const char *path){
	char *buf = NULL;
	if(!path ||
	   !(buf = calloc(1 , (sizeof(*buf) * strlen(path)) + 30 ))){
		return NULL;
       	}
	sprintf(buf , "%s/updatedeployqt.json" , path);
	return buf;
}

/*
 * @desc   : this prints the version and other things about the program.
 * @param  : nothing.
 * @returns: nothing.
*/
static void print_header(){
	printf("updatedeployqt git-commit %s , built on %s\n",
#ifdef GIT_COMMIT_STR
           GIT_COMMIT_STR
#else
           "none"
#endif
           ,
           __TIMESTAMP__
          );
    printf("Copyright (C) 2019 The Future Shell Laboratory.\n");
}

/* 
 * @desc   : this prints the usage to standard output.
 * @param  : program name as char*
 * @returns: nothing.
*/
static void print_help(char *program_name){
    printf("\nUsage: %s [OPTIONS] [PATH TO DEPLOY DIR]\n\n", program_name);
    printf("OPTIONS: \n");
    printf("    -c,--config           configuration file to use for the deploy.(default=.)\n");
    printf("    -g,--generate-config  create configuration file interactively.\n");
    printf("    -q,--quiet            do not print anything to stdout.\n");
    printf("    -v,--version          show version and exit.\n");
}
