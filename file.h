#define		MAX_STRING_LENGTH	100
#define		FILE_OPEN_ERROR		0
#define		OK					1
#define		ADD_MOTD_LINE		3


int fgetline(FILE* infile, char* buffer);

int process_assault_ini_file();		 // assault mod
int process_bloodmoney_ini_file();	 // blood money mod v616
int process_botmatch_ini_file();	 // botmatch mod v0.39
int process_botmen_ini_file();		 // botmen
int process_comp_ini_file();		 // monkey mod 2.0c
int process_coop_ini_file();		 // coop mod
int process_curse_ini_file();		 // curse mod
int process_dogmod_ini_file();		 // dog mod
int process_genocide_ini_file();	 // genocide mod
int process_godfather_ini_file();	 // godfather mod
int process_gunrace_ini_file();		 // gunrace mod
int process_hitmen_ini_file();		 // hitmen mod
int process_hitmen_teams_ini_file(); // hitmen teams mod
int process_insanity_ini_file();	 // insanity mod
int process_insomnia_ini_file();	 // insomina mod
int process_kpq2_ini_file();		 // kpq2 mod
int process_kpz_ini_file();			 // kpz mod
int process_sqm_ini_file();			 // sgm mod

int file_exist(const char *file);
