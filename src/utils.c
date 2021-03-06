#include <logger.h>
#include <utils.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *get_bundled_data_file(const char *file){
	char *p = NULL;
	char *dir = calloc(1 , sizeof(*dir) * 100);	
	if(!dir){
		return NULL;
	}
	
	if (readlink("/proc/self/exe", dir , 100) < 0) {
                free(dir);    
		return NULL;
	}
	
	if((p = strstr(dir , "/usr/"))){
		*p = '\0';
	}

	p = calloc(1 , sizeof(*p) * (strlen(dir) + 10 + strlen(file)));
	if(!file){
		sprintf(p , "%s/data/" , dir);
	}else{
		sprintf(p , "%s/data/%s" , dir , file);
	}
	free(dir);
	return p;	
}


int read_bytes(FILE *fp , char **buffer , size_t n){
	long int pos = 0;
	int r = 0;
	pos = ftell(fp); /* Make it as if one byte is only read. */
	r = fread(*buffer , sizeof(**buffer) , n , fp);
	fseek(fp , pos , SEEK_SET);
	if(getc(fp) == EOF){
		return 0; 
	}
	return (r<=0) ? 0 : r;
}

int find_offset_and_write(FILE *fp, const char *to_find, const char *replace, size_t to_write){
    char *buffer = NULL;
    int r = 0;
    buffer = calloc(1 , sizeof(*buffer) * to_write);
    if(!buffer){
	    return -1;
    }
    if(!replace || !to_find || !fp) {
        free(buffer);
	return -1;
    }

    rewind(fp);

    /* Check if we have an empty file. */
    if(feof(fp)) {
	free(buffer);
        return -1;
    }

    while(1) {
        memset(buffer, 0, sizeof(*buffer) * to_write);
        if(read_bytes(fp , &buffer , to_write) == 0){
		break;
	}
        if(!strncmp(buffer, to_find , to_write)) {
            fseek(fp, (ftell(fp) - 1), SEEK_SET);
            fwrite(replace, sizeof *replace, to_write, fp);
            ++r;
        }
    }
    free(buffer);
    return r;
}

int copy_file(const char *dest , const char *src){
	if(!dest || !src){
		return -1;
	}
	FILE *dest_fp;
        FILE *src_fp;
        int c = 0;

        if(!(src_fp=fopen(src,"rb"))) {
                printl(fatal, "cannot open '%s' for reading", src);
                return -1;
        }

        if(!(dest_fp=fopen(dest, "wb"))) {
                printl(fatal, "cannot open '%s' for writing", dest);
                fclose(src_fp);
		return -1;
	}

        while((c = getc(src_fp)) != EOF) {
                putc(c, dest_fp);
        }
        fclose(src_fp);
        fclose(dest_fp);
	return 0;
}

int write_string_as_file(const char *str , size_t str_len , const char *path){
	if(!str || !path){
		return -1;
	}
	FILE *fp;
	size_t iter = 0;

	if(!(fp=fopen(path, "wb"))){
		printl(fatal , "cannot open '%s' for writing" , path);
		fclose(fp);
		return -1;
	}

	while(iter < str_len){
		putc(str[iter] , fp);
		++iter;
	}
	fclose(fp);
	return 0;
}
