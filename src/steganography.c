#include "steganography.h"

#define SUPPORTED_FILE_TYPES {"png"}
#define PROGRAM_VERSION "0.5"
#define PROGRAM_NAME "rbsteg"

#define BOLD_BLUE  "\033[1m\033[34m"
#define BOLD_RED "\033[1m\033[31m"
#define BOLD_GREEN "\033[1m\033[32m"
#define RESET "\033[0m"

static int verbose = 1;

void print_status(FILE *out, const char *fmt, ...)
{
	if(verbose){ 
		fprintf(stderr, BOLD_BLUE "status: " RESET);
		va_list ap;
	    va_start(ap, fmt);
	    vfprintf(out, fmt, ap);
	    va_end(ap);	
	}   
}

void print_error(FILE *out, const char *fmt, ...)
{
	if(verbose){ 
		fprintf(stderr, BOLD_RED "error: " RESET);
		va_list ap;
	    va_start(ap, fmt);
	    vfprintf(out, fmt, ap);
	    va_end(ap);	
	}   
}

void print_success(FILE *out, const char *fmt, ...){
	if(verbose){ 
		fprintf(stderr, BOLD_GREEN "success: " RESET);
		va_list ap;
	    va_start(ap, fmt);
	    vfprintf(out, fmt, ap);
	    va_end(ap);	
	}   
}

static const char* get_filename_ext(const char *filename) 
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

static void print_version()
{
	fprintf(stderr, "%s is a program created by Henri Schmidt.\ncurrently running version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
}

static void print_usage()
{
	const char* supported_files[] = SUPPORTED_FILE_TYPES;

	fprintf(stderr, "Usage: %s [-t filetype] -m encryption | decryption -k encryptionkey -i inputfile [< | > datastream]\n", PROGRAM_NAME);
	fprintf(stderr, "\tCurrently supported file types:");

	for(unsigned long int i = 0; i < sizeof(supported_files) / sizeof(supported_files[0]); i++){
		fprintf(stderr, " %s", supported_files[i]);
	}

	fprintf(stderr, "\nUsage example:\n");
	fprintf(stderr, "\tEncrypt data from input.txt in png test.png: %s -m encryption -k gfhjkm -i test.png < input.txt\n", PROGRAM_NAME);
	fprintf(stderr, "\tDecrypt data from test.png and put output in output.txt: %s -m decryption -k gfhjkm -i test.png > output.txt\n", PROGRAM_NAME);
}

static int init_decrypting_data(const char* type, const char* inputfile)
{
	int ret = 0;

	if(strcmp(type, "png") == 0){
		print_status(stderr, "attempting to initialize png...\n");
		if((ret = init_png_file(inputfile)) == 0){
			print_success(stderr, "successfully initialized png.\n");
			print_status(stderr, "currently attempting decryption...\n");
			if((ret = decrypt_png_file()) == 0){
				print_success(stderr, "sucessfully decrypted data from %s\n", inputfile);
			}
		}
	}else{
		print_error(stderr, "undetected file type: %s\nplease specify a known file type\n", type);
		ret = 1;
	}

	return ret;
}

static int init_encrypting_data(const char* type, const char* inputfile)
{
	int ret = 0;

	if(strcmp(type, "png") == 0){
		print_status(stderr, "attempting to initialize png...\n");
		if((ret = init_png_file(inputfile)) == 0){
			print_success(stderr, "successfully initialized png.\n");
			print_status(stderr, "currently attempting encryption...\n");
			if((ret = encrypt_png_file()) == 0){
				print_success(stderr, "sucessfully encrypted data into %s\n", inputfile);
			}
		}
	}else{
		print_error(stderr, "undetected file type: %s\nplease specify a known file type\n", type);
		ret = -1;
	}

	return ret;
}

int main(int argc, char **argv)
{	
	const char* type = NULL, *inputfile = NULL, *mode = NULL;
	char key[1024] = {0};
	int c = -1;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0){
			print_usage();
			return -1;
		}else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-version") == 0){
			print_version();
			return -1;
		}
	}

	while((c = getopt(argc, argv, "m:t:i:k:hv")) != -1){
		switch(c){
			case 'k':
				if(strlen(optarg) < 1024){
					memcpy(key, optarg, strlen(optarg));
				}else{
					memcpy(key, optarg, 1024);
				}
				break;
			case 'm':
				mode = optarg;
				break;
			case 't':
				type = optarg;
				break;
			case 'i':
				inputfile = optarg;
				break;
			case 'v':
				print_version();
				return -1;
				break;
			case 'h':
				print_usage();
				return -1;
				break;
			case '?':
				print_usage();
				return -1;
				break;
			default:
				print_error(stderr, "error parsing arguments.");
		}
	}

	if(inputfile == 0 || key == 0 || mode == 0){
		print_usage();
		return -1;
	}

	if(type == 0){
		type = get_filename_ext(inputfile);
	}

	set_key(key, 1024);

	for(int i = 0; i < 10; i++){
    	isaac(0, 0);
  	}

	if(strcmp(mode, "encryption") == 0){
		if(isatty(STDIN_FILENO)){
			print_usage();
			return -1;
		}

		return init_encrypting_data(type, inputfile);
	}
	else if(strcmp(mode, "decryption") == 0){
		if(isatty(STDOUT_FILENO)){
			print_usage();
			return -1;
		}

		return init_decrypting_data(type, inputfile);
	}else{
		print_usage();
		return -1;
	}
}