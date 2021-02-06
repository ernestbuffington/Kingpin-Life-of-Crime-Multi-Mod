#define		MAX_STRING_LENGTH	100
#define		FILE_OPEN_ERROR		0
#define		OK					1
#define		ADD_MOTD_LINE		3

int fgetline(FILE* infile, char* buffer);
int process_botmatch_ini_file();
int file_exist(const char *file);
