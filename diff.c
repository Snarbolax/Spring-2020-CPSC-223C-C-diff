#include "diff.h"

version = false;
brief = false;
report = false;
reportQ false;
defaultdiff = true;
sidebyside = false;
leftcolumn = false;
suppresscommon = false;

countL = 0;
countR = 0;

char* strdup(const char *s)   /* make a duplicate of s */
{
	char *p;
	p = (char *)malloc(strlen(s) + 1);  /* +1 for Œ\0Œ */
	if (p != NULL)
		strcpy(p, s);
	return p;
}

void stream_to_buf(void) { // moves standard input into buff[]
	//int s = 0;
	//char c = fgetc(stdin);

	memset(buff, NULL, sizeof(options));
	fgets(buff, BUFLEN, stdin);

	/*while (c != EOF) {
		buff[s] = c;
		++s;
		c = fgetc(stdin);
	}*/
}

void buf_to_tok(void) {
	bool fdone = false;
	bool fdone2 = false;

	char temp[BUFLEN];

	memset(temp, NULL, sizeof(temp));
	memset(options, NULL, sizeof(options));
	memset(buff2, NULL, sizeof(buff2));
	memset(leftname, NULL, sizeof(leftname));
	memset(rightname, NULL, sizeof(rightname));

	strcpy(buff2, buff);

	char* tok;
	tok = strtok(buff2, " "); // gets name of the diff executable from stdin
	tok = strtok(NULL, " "); // moves past the name of the diff executable from stdin, because it won't be used

	int zwei = 0;

	while (tok != NULL) {
		if (*tok != "-") { // rule is that files can't start with a "-"
						   // as a result, the Right File doesn't have to be immediately be declared right after the Left File
						   // The first valid file will be considered as the Left File, the second will be the Right File
			if (fdone == false) { // if Left candidate isn't valid
				fileLEFT = fopen(tok, "r");
				leftname = strdup(tok);
				if (fileLEFT != NULL) { fdone = true; }  // if valid, allow code to consider Right candidates
				else {
					fprintf(stderr, "Error: Left file %s can't be found.\n", leftname);
					exit(1);
				}
			}
			else if (fdone == true && fdone2 == false) { // if Left candidate is valid and Right candidate isn't found yet
				fileRIGHT = fopen(tok, "r");
				rightname = strdup(tok);
				if (fileRIGHT != NULL) { fdone2 = true; }
				else {
					fclose(fileLEFT);
					fprintf(stderr, "Error: Right file %s can't be found.\n", rightname);
					exit(1);
					//fdone = false; // restart file finding process
				}
			}
			else { fprintf(stderr, "Error: Too many file candidates.\n"); exit(1); }
		}
		else if (*tok == "-") { // only options start with a "-"
			strcat(options, tok);
			strcat(options, " ");
			++zwei;
		}
		tok = strtok(NULL, " ");
	}

	get_options();
	if (zwei >= OPTIONS_MAX) {
		fprintf(stderr, "Error: Too many options.\n");
		exit(1);
	}
	else if (version == false) {
		if (fdone == false || fdone2 == false) {
			fprintf(stderr, "Error: Left file or right file can't be found.\n");
			exit(1);
		}
	}
}

void files_to_buff(void) {
	memset(leftF, NULL, sizeof(leftF));
	memset(rightF, NULL, sizeof(rightF));
	fgets(leftF, BUFLEN, fileLEFT);
	fgets(rightF, BUFLEN, fileRIGHT);
}

void get_options(void) {
	memset(buff2, NULL, sizeof(buff2));
	strcpy(buff2, options);

	bool combo = false; // for -q -s
	bool combo2 = false; // for -y --left-column or -y --side-by-side
	int cord = 0;

	char* tok;
	tok = strtok(buff2, " ");

	while (tok != NULL || cord < OPTIONS_MAX) {
		if (strcmp(tok, "-v") == 0 || strcmp(tok, "--version") == 0) { version = true; }
		else if (strcmp(tok, "-q") == 0 || strcmp(tok, "--brief") == 0) { brief = true; combo = true; }
		else if (strcmp(tok, "-s") == 0 || strcmp(tok, "--report-identical-files") == 0) { report = true; if (combo == true) { reportQ = true; } }
		else if (strcmp(tok, "-y") == 0 || strcmp(tok, "--side-by-side") == 0) { combo2 = true; }
		else if (strcmp(tok, "--left-column") == 0) { if (combo2 == true) { leftcolumn = true; } else { fprintf(stderr, "Error: Invalid option syntax.\n"); exit(1); } }
		else if (strcmp(tok, "--suppress-common-lines") == 0) { if (combo2 == true) { suppresscommon = true; } else { fprintf(stderr, "Error: Invalid option syntax.\n"); exit(1); } }
		else if (strcmp(tok, "--normal") == 0 || strlen(buff2) == 0) { defaultdiff = true; }

		++cord;
		tok = strtok(NULL, " ");
	}

	if (version == true || brief == true || report == true || reportQ == true || sidebyside == true || leftcolumn == true || suppresscommon == true) { defaultdiff = false; }
	if ((version == true && brief == true) || (version == true && report == true) || (version == true && sidebyside == true) || (brief == true && sidebyside == true) || (report == true && sidebyside == true)) {
		fprintf(stderr, "Error: Invalid option combination.\n"); exit(1);
	}
}

void diff_version(void) {
	//char print_version[BUFLEN] = { "0.1" };
	float print_version = 0.1;
	char print_author[BUFLEN] = { "Kenneth Doan" };
	char print_license[] = { "MIT LICENSE\n\nCopyright (c) 2020 %s\n\nPermission is hereby granted, free of charge, to any person obtaining a copy\nof this software and associated documentation files (the \"Software\"), to deal\nin the Software without restriction, including without limitation the rights\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\ncopies of the Software, and to permit persons to whom the Software is\nfurnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.", print_author };

	printf("\n\nVersion: %f\n\nLicense: %s\n\nAuthor: %s\n\n", print_version, print_license, print_author);
}


char* get_line(int count, char v) {
	char c;
	char new[BUFLEN];
	int newc = 0;
	memset(new, NULL, sizeof(new));
	if (v = 'l') {
		if (count >= strlen(leftF)) { new[0] = '\0'; return new; }
		else { ++count; }
	}
	else if (v = 'r') {
		if (count >= strlen(rightF)) { new[0] = '\0'; return new; }
		else { ++count; }
	}

	if (v = 'l') { c = leftF[count]; }
	else if (v = 'r') { c = rightF[count]; }
	while (c != '\n' || c != EOF) {
		new[newc] = c;
		++newc;
		++count;
		if (v = 'l') { c = leftF[count]; }
		else if (v = 'r') { c = rightF[count]; }
	}
	new[newc] = '\n';
	++newc;
	new[newc] = '\0';
	if (v == 'l') { countL = count; }
	else if (v == 'r') { countR = count; }

	return new;
}

void defdiff(void) { // default order L -> R
	bool del = true;
	bool ins = true;

	countL = 0;
	countR = 0;

	for (; countL < strlen(leftF);) { // focuses on deletions, L -> R. Lines from L, gone in R.
		strcpy(line_left, get_line(countL, 'l'));
		for (; countR < strlen(rightF);) {
			strcpy(line_right, get_line(countR, 'r'));
			if (strcmp(line_left, line_right) == 0) { del = false; countR = strlen(rightF); } // match found. Deletion false.
		}
		if (del == true) { printf("< %s", line_left); }
		else { del = true; }
		countR = 0;
	}

	countL = 0;
	countR = 0;

	for (; countR < strlen(rightF);) { // focuses on insertions, R -> L. Lines from R, not in L.
		strcpy(line_right, get_line(countR, 'r'));
		for (; countL < strlen(leftF);) {
			strcpy(line_left, get_line(countL, 'l'));
			if (strcmp(line_right, line_left) == 0) { ins = false; countL = strlen(leftF); } // match found. Insertion false.
		}
		if (ins == true) { printf("> %s", line_right); }
		else { ins = true; }
		countL = 0;
	}
	//int max = strlen(line_left) + strlen(line_right);
}

void diff_brief(void) {
	if (strcmp(leftF, rightF) != 0) { printf("Files %s and %s differ\n", leftname, rightname); }
}

void diff_report(void) {
	if (strcmp(leftF, rightF) == 0) { printf("Files %s and %s are identical\n", leftname, rightname)
	else { defdiff(); }
}

void diff_reportq(void) {
	if (strcmp(leftF, rightF) == 0) { printf("Files %s and %s are identical\n", leftname, rightname); }
	else if (strcmp(leftF, rightF) != 0) { printf("Files %s and %s differ\n", leftname, rightname); }
}

void diff_side(void) { //todo
	bool del = true;
	bool ins = true;

	char difcand[BUFLEN];

	bool trigger = false;
	bool common = false;

	countL = 0;
	countR = 0;

	for (; countL < strlen(leftF);) { // focuses on deletions, L -> R. Lines from L, gone in R.
		strcpy(line_left, get_line(countL, 'l'));
		for (; countR < strlen(rightF);) {
			strcpy(line_right, get_line(countR, 'r'));
			if (strcmp(line_left, line_right) == 0) {
				common = true;

				del = false;
				countR = strlen(rightF); 
				printf("%s", line_left);
			} // match found. Deletion false.
			else {
				if (common == true) { trigger = true; common = false; }
				else if (trigger == true) { printf("| %s", line_left); }
			}
			if (isspace(line_left[0]) != 0) {
				char c = line_left[0];
				bool isa = true;
				int cada = 0;
				do {
					if (isspace(c) == 0) { isa = false; }
					++cada;
					c = line_left[cada];
				} while (isa == true || cada < strlen(line_left));
				if (isa == true) { trigger = false; };
			}
		}
		if (del == true) { printf("< %s", line_left); }
		else { del = true; }
		countR = 0;
	}

	countL = 0;
	countR = 0;

	for (; countR < strlen(rightF);) { // focuses on insertions, R -> L. Lines from R, not in L.
		strcpy(line_right, get_line(countR, 'r'));
		for (; countL < strlen(leftF);) {
			strcpy(line_left, get_line(countL, 'l'));
			if (strcmp(line_right, line_left) == 0) { ins = false; countL = strlen(leftF); } // match found. Insertion false.
		}
		if (ins == true) { printf("> %s", line_right); }
		else { ins = true; }
		countL = 0;
	}
}

void diff_leftc(void) { //todo
	return void; // I didn't design the program to print out more than one column. uh oh
}

void diff_supp(void) { //todo
	bool del = true;
	bool ins = true;

	char difcand[BUFLEN];

	bool trigger = false;
	bool common = false;

	countL = 0;
	countR = 0;

	for (; countL < strlen(leftF);) { // focuses on deletions, L -> R. Lines from L, gone in R.
		strcpy(line_left, get_line(countL, 'l'));
		for (; countR < strlen(rightF);) {
			strcpy(line_right, get_line(countR, 'r'));
			if (strcmp(line_left, line_right) == 0) {
				common = true;

				del = false;
				countR = strlen(rightF);
				//printf("%s", line_left);
			} // match found. Deletion false.
			else {
				if (common == true) { trigger = true; common = false; }
				else if (trigger == true) { printf("| %s", line_left); }
			}
			if (isspace(line_left[0]) != 0) {
				char c = line_left[0];
				bool isa = true;
				int cada = 0;
				do {
					if (isspace(c) == 0) { isa = false; }
					++cada;
					c = line_left[cada];
				} while (isa == true || cada < strlen(line_left));
				if (isa == true) { trigger = false; };
			}
		}
		if (del == true) { printf("< %s", line_left); }
		else { del = true; }
		countR = 0;
	}

	countL = 0;
	countR = 0;

	for (; countR < strlen(rightF);) { // focuses on insertions, R -> L. Lines from R, not in L.
		strcpy(line_right, get_line(countR, 'r'));
		for (; countL < strlen(leftF);) {
			strcpy(line_left, get_line(countL, 'l'));
			if (strcmp(line_right, line_left) == 0) { ins = false; countL = strlen(leftF); } // match found. Insertion false.
		}
		if (ins == true) { printf("> %s", line_right); }
		else { ins = true; }
		countL = 0;
	}
}

void close_files(FILE* LEFT, FILE* RIGHT) { fclose(LEFT); fclose(RIGHT); }

void branch(void) {
	if (version == true) { diff_version(); }
	else if (brief == true) {
		if (report == true) { diff_reportq(); }
		else { diff_brief(); }
	}
	else if (report == true) { diff_report(); }
	else if (sidebyside == true) {
		if (leftcolumn == true) { diff_leftc(); } //todo
		else if (suppresscommon == true) { diff_supp(); }
		else { diff_side(); }
	}
	else if (defaultdiff == true) { defdiff(); }
}

void diff(void) {
	stream_to_buf();
	buf_to_tok();
	files_to_buff();
	branch();

	close_files(fileLEFT, fileRIGHT);
}

int main(int argc, const char* argv[]) {
	diff();
	return 0;
}