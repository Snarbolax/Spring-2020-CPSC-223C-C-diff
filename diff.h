#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFLEN 512
#define SIZE(x) (sizeof(x)/sizeof(*(x)))
#define OPTIONS_MAX 2 // maximum options

FILE* fileLEFT;
FILE* fileRIGHT;

bool version;
bool brief;
bool report;
bool reportQ;
bool defaultdiff;
bool sidebyside;
bool leftcolumn;
bool suppresscommon;

char* strdup(const char *s);

void stream_to_buf(void);
void buf_to_tok(void);
void files_to_buff(void);
void get_options(void);
void diff_version(void);
char* get_line(int count, char v);
void defdiff(void);
void diff(void);
void diff_brief(void);
void diff_report(void);
void diff_reportq(void);
void diff_side(void);
void diff_leftc(void);
void diff_supp(void);

void branch(void);

int countL;
int countR;
char* strdup(const char *s);
char buff[BUFLEN];
char buff2[BUFLEN];
char options[BUFLEN];

char leftF[BUFLEN];
char rightF[BUFLEN];
char leftname[BUFLEN];
char rightname[BUFLEN];
char line_left[BUFLEN];
char line_right[BUFLEN];

char** options_queue[BUFLEN];
int queue_iterator = 0;

//void check_options(char** argv);
//void get_options(int iterator, char** argv);

void open_files(char** argv);
void close_files(FILE* LEFT, FILE* RIGHT);