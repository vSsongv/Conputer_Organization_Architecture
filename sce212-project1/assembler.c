#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/******************************************************
 * Structure Declaration
 *******************************************************/

struct Inst {
    char *name;
    char *op;
    char type;
    char *funct;
};

struct Data {
    int value;
    struct Data *next;
};

struct Text {
    int idx;
    char *d;
    char *s;
    char *t;
    unsigned int address;
    struct Text *next;
};

struct Sym {
    char *name;
    int size;
    unsigned int address;
    struct Data *first;
    struct Sym *next;
    struct Data *last;
};


/******************************************************
 * Global Variable Declaration
 *******************************************************/

struct Inst inst[20] = {
    {"addiu",   "001001", 'I', ""},
    {"addu",    "000000", 'R', "100001"},
    {"and",     "000000", 'R', "100100"},
    {"andi",    "001100", 'I', ""},
    {"beq",     "000100", 'I', ""},
    {"bne",     "000101", 'I', ""},
    {"j",       "000010", 'J', ""},
    {"jal",     "000011", 'J', ""},
    {"jr",      "000000", 'R', "001000"},
    {"lui",     "001111", 'I', ""},
    {"lw",      "100011", 'I', ""},
    {"nor",     "000000", 'R', "100111"},
    {"or",      "000000", 'R', "100101"},
    {"ori",     "001101", 'I', ""},
    {"sltiu",   "001011", 'I', ""},
    {"sltu",    "000000", 'R', "101011"},
    {"sll",     "000000", 'S', "000000"},
    {"srl",     "000000", 'S', "000010"},
    {"sw",      "101011", 'I', ""},
    {"subu",    "000000", 'R', "100011"} 
};
struct Sym *Symbols;
struct Text *Texts;
int datasize, textsize;


/******************************************************
 * Function Declaration
 *******************************************************/

/* You may need the following functions */
char* change_file_ext(char *str);
char* num_to_bits(unsigned int num, int len);

/* You may fill the following functions */
void read_asm();
void subst_asm_to_num();
void print_bits();


/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/
int main(int argc, char* argv[]) {

    FILE *input, *output;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // reading the input file
    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // creating the output file (*.o)
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if(change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = freopen(filename, "w", stdout);
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    Symbols = (struct Sym *)malloc(sizeof(struct Sym));
    Texts = (struct Text *)malloc(sizeof(struct Text));

    /******************************************************
     *  Let's complete the below functions!
     *
     *  read_asm(): Read the assmebly code.
     *  subst_asm_to_num(): Convert the assembly code to num.
     *  print_bits(): Print the results of assemble.
     *
     *******************************************************/
    read_asm();
    subst_asm_to_num();
    print_bits();

    // freeing the memory
    free(filename);
    free(Symbols);
    free(Texts);

    fclose(input);
    fclose(output);
    exit(EXIT_SUCCESS);
}

/******************************************************
 * Function: change_file_ext
 *
 * Parameters:
 *  char
 *      *str: a raw filename (without path)
 *
 * Return:
 *  return NULL if a file is not an assembly file.
 *  return empty string
 *
 * Info:
 *  This function reads filename and converts it into
 *  object extention name, *.o
 *
 *******************************************************/
char* change_file_ext(char *str) {
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

/******************************************************
 * Function: num_to_bits
 *
 * Parameters:
 *  unsigned int
 *      num: a decimal number to be converted to binary bits
 *  int
 *      len: a length of bits
 *
 * Return:
 *  return string of binary bits
 *
 * Info:
 *  This function reads an integer number and converts it into
 *  binary bits represented by string.
 *
 *******************************************************/
char* num_to_bits(unsigned int num, int len) {
    char* bits = (char *) malloc(len+1);
    int idx = len-1, i;

    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        } else { 
            bits[idx--] = '0';
        }
        num /= 2;
    }

    for (i = idx; i >= 0; i--){ 
        bits[i] = '0';
    }

    return bits;
}


/******************************************************
 * Function: read_asm
 *
 * Info:
 *  This function reads assembly code and saves the
 *  information into struct Sym, Data, Text, respectively.
 *
 *******************************************************/
void read_asm() {
	int tmp, i;
	unsigned int address;
	char temp[0x1000] = { 0 };
	struct Sym *temp_sym = NULL;
	struct Data *temp_data = NULL;
	struct Text *temp_text = NULL;
	temp_sym = Symbols;
	temp_text = Texts;
	//Read .data region
	address = 0x10000000;
	for (i = 0; scanf("%s", temp) == 1;) {
		if (strcmp(temp, ".text") == 0) {
			break;

		}
		else if (temp[strlen(temp) - 1] == ':') {
			(*temp_sym).next = (struct Sym *)malloc(sizeof(struct Sym));
			temp_sym = (*temp_sym).next;
			(*temp_sym).next = NULL;
			(*temp_sym).address = address;
			(*temp_sym).size = 0;
			(*temp_sym).first = (struct Data *)malloc(sizeof(struct Data));
			temp_data = (*temp_sym).first;
			(*temp_data).next = NULL;
			int len = strlen(temp);
			((*temp_sym).name) = (char *)malloc(sizeof(char)*len);
			strncpy((*temp_sym).name, temp, len - 1);
			*((*temp_sym).name + len - 1) = '\0';

		}
		else if (strcmp(temp, ".word") == 0) {
			(*temp_data).next = (struct Data *)malloc(sizeof(struct Data));
			temp_data = (*temp_data).next;
			(*temp_data).next = NULL;
			(*temp_sym).last = temp_data;
			scanf("%s", temp);
			(*temp_data).value = (int)strtol(temp, NULL, 0);
			(*temp_sym).size++;
			address += 4;
		}
	}

	datasize = address - 0x10000000;

	//Read .text region
	address = 0x400000;
	for (i = 0; scanf("%s", temp) == 1;) {
		if (temp[strlen(temp) - 1] == ':') {
			(*temp_sym).next = (struct Sym *)malloc(sizeof(struct Sym));
			temp_sym = (*temp_sym).next;
			(*temp_sym).next = NULL;
			(*temp_sym).address = address;
			(*temp_sym).size = 0;
			(*temp_sym).first = NULL;
			(*temp_sym).last = NULL;
			int len = strlen(temp);
			((*temp_sym).name) = (char *)malloc(sizeof(char)*len);
			strncpy((*temp_sym).name, temp, len - 1);
			*((*temp_sym).name + len - 1) = '\0';

		}
		else {
			int tlen = 0;
			int j = 0;
			(*temp_text).next = (struct Text *)malloc(sizeof(struct Text));
			temp_text = (*temp_text).next;
			(*temp_text).next = NULL;
			(*temp_text).address = address;
			(*temp_sym).size++;
			for (j; j < 20; j++)
			{
				if (strcmp(temp, inst[j].name) == 0)
				{
					if (strcmp(inst[j].name, "lw") == 0 || strcmp(inst[j].name, "sw") == 0)
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).t = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).t, temp, tlen + 1);

						int s = 0;
						scanf("%s", temp);

						(*temp_text).d = (char *)malloc(sizeof(char) * 7);
						for (; temp[s] != '('; s++)
						{

							(*temp_text).d[s] = temp[s];
						}
						(*temp_text).d[s] = '\0';
						s++;

						int r = 0;
						(*temp_text).s = (char *)malloc(sizeof(char) * 4);
						for (; temp[s] != ')'; s++, r++)
						{
							(*temp_text).s[r] = temp[s];
						}
						(*temp_text).s[r] = '\0';

						address += 4;
					}

					else if (strcmp(inst[j].name, "lui") == 0)
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).t = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).t, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).d = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).d, temp, tlen + 1);

						address += 4;
					}

					else if (strcmp(inst[j].name, "jr") == 0)
					{
						(*temp_text).idx = j;
						
						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).s = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).s, temp, tlen + 1);

						address += 4;

					}

					else if (inst[j].type == 'R')
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).d = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).d, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).s = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).s, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).t = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).t, temp, tlen + 1);
						address += 4;

					}
					else if (inst[j].type == 'I')
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).t = (char *)malloc(sizeof(char)*(tlen + 1));
						strncpy((*temp_text).t, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).s = (char *)malloc(sizeof(char)*tlen + 1);
						strncpy((*temp_text).s, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).d = (char *)malloc(sizeof(char)*tlen + 1);
						strncpy((*temp_text).d, temp, tlen + 1);

						address += 4;

					}
					else if (inst[j].type == 'S')
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).d = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).d, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).t = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).t, temp, tlen + 1);

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).s = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).s, temp, tlen + 1);

						address += 4;

					}
					else if (inst[j].type == 'J')
					{
						(*temp_text).idx = j;

						scanf("%s", temp);
						tlen = strlen(temp);
						(*temp_text).d = (char *)malloc(sizeof(char)*tlen);
						strncpy((*temp_text).d, temp, tlen + 1);

						address += 4;

					}
					break;
				}
			}
			if (j == 20)
			{
				scanf("%s", temp);
				tlen = strlen(temp);

				char sym_name[100] = { 0 };
				scanf("%s", sym_name);

				struct Sym *sym_ptr;
				unsigned int tadd;
				char adtemp[6] = { NULL };
				int adlen = 0;

				for (sym_ptr = Symbols; (*sym_ptr).next != NULL;)
				{
					sym_ptr = (*sym_ptr).next;
					if (strcmp(sym_name, (*sym_ptr).name) == 0)
					{
						tadd = (*sym_ptr).address;
						unsigned int check = tadd & 0x0000ffff;

						if (check == 0x0000)
						{
							(*temp_text).idx = 9;

							(*temp_text).t = (char *)malloc(sizeof(char)*(tlen + 1));
							strncpy((*temp_text).t, temp, tlen + 1);
							tadd = tadd >> 16;
							sprintf(adtemp, "%d", tadd);
							adlen = strlen(adtemp);
							(*temp_text).d = (char *)malloc(sizeof(char)*(adlen + 1));
							strncpy((*temp_text).d, adtemp, (adlen + 1));
							address += 4;

						}
						else
						{
							(*temp_text).idx = 9;

							(*temp_text).t = (char *)malloc(sizeof(char)*(tlen + 1));
							strncpy((*temp_text).t, temp, tlen + 1);
							tadd = tadd >> 16;
							sprintf(adtemp, "%d", tadd);
							adlen = strlen(adtemp);
							(*temp_text).d = (char *)malloc(sizeof(char)*(adlen + 1));
							strncpy((*temp_text).d, adtemp, (adlen + 1));
							address += 4;

							(*temp_text).next = (struct Text *)malloc(sizeof(struct Text));
							temp_text = (*temp_text).next;
							(*temp_text).next = NULL;
							(*temp_text).address = address;
							(*temp_sym).size++;
							(*temp_text).idx = 13;

							(*temp_text).t = (char *)malloc(sizeof(char)*(tlen + 1));
							strncpy((*temp_text).t, temp, tlen + 1);

							(*temp_text).s = (char *)malloc(sizeof(char)*(tlen + 1));
							strncpy((*temp_text).s, temp, tlen + 1);

							sprintf(adtemp, "%d", check);
							adlen = strlen(adtemp);
							(*temp_text).d = (char *)malloc(sizeof(char)*(adlen + 1));
							strncpy((*temp_text).d, adtemp, (adlen + 1));
							address += 4;

						}
						break;
					}
				}
			}
		}
	}
	textsize = address - 0x400000;
}



/******************************************************
 * Function: subst_asm_to_num
 *
 * Info:
 *  This function converts assembly code to numbers.
 *
 *******************************************************/
void subst_asm_to_num() {
    struct Text *text;
	
    for (text = Texts->next; text != NULL; text = text->next) {

        if((*text).idx == 4 ||(*text).idx == 5){
			char *btemp;
			btemp = (*text).t;
			(*text).t = (*text).s;
			(*text).s = btemp;
			(*text).s = num_to_bits(atoi(((*text).s + 1)),5);
			(*text).t = num_to_bits(atoi(((*text).t + 1)),5);
			struct Sym *sym_finder;
			int badd = 0;
			for (sym_finder = Symbols; (*sym_finder).next != NULL;)
				{
					sym_finder = (*sym_finder).next;

					if (strcmp((*text).d, (*sym_finder).name) == 0)
					{
					 	badd = ((*sym_finder).address - (*text).address - 4) / 4;
					}
				}
			(*text).d = num_to_bits(badd,16);

		} // case of beq, ben
		else if((*text).idx == 8){

			(*text).s = num_to_bits(atoi(((*text).s + 1)),5);
			(*text).t = num_to_bits(0,5);
			(*text).d = num_to_bits(0,5);

		} // case of jr
		else if((*text).idx == 9)
		{
			(*text).s = num_to_bits(0,5);
			(*text).t = num_to_bits(atoi(((*text).t + 1)),5);
			(*text).d = num_to_bits(strtol((*text).d,NULL,0),16);
			
		} // case of lui
		else if(inst[(*text).idx].type == 'J')
		{
			struct Sym *sym_finder;
			unsigned int jadd = 0;
			for (sym_finder = Symbols; (*sym_finder).next != NULL;)
				{
					sym_finder = (*sym_finder).next;

					if (strcmp((*text).d, (*sym_finder).name) == 0)
					{
					 	jadd = (*sym_finder).address / 4;
					}
				}
			(*text).d = num_to_bits(jadd,26);
		} // case of J, Jal 
		else if(inst[(*text).idx].type == 'R')
		{
			(*text).d = num_to_bits(atoi(((*text).d + 1)),5);
			(*text).s = num_to_bits(atoi(((*text).s + 1)),5);
			(*text).t = num_to_bits(atoi(((*text).t + 1)),5);
		} //case of R type
		else if(inst[(*text).idx].type == 'I')
		{
			(*text).d = num_to_bits(strtol((*text).d,NULL,0),16);
			(*text).s = num_to_bits(atoi(((*text).s + 1)),5);
			(*text).t = num_to_bits(atoi(((*text).t + 1)),5);
		} //case of I type
		else if(inst[(*text).idx].type == 'S')
		{
			(*text).d = num_to_bits(atoi(((*text).d + 1)),5);
			(*text).s = num_to_bits(strtol((*text).s,NULL,0),5);
			(*text).t = num_to_bits(atoi(((*text).t + 1)),5);
		} //case of S type
    }
}


/******************************************************
 * Function: print_bits
 *
 * Info:
 *  This function prints binary bit string converted by
 *  assembler. It will make binary object file of assembly code.
 *
 *******************************************************/
void print_bits() {
    struct Text* text;
    struct Sym* sym;
    struct Data* data;
    
    // print the header
    printf("%s", num_to_bits(textsize, 32));
    printf("%s", num_to_bits(datasize, 32));

    // print the body
    for (text = Texts->next ; text != NULL ; text = text->next) {
        printf("%s", inst[text->idx].op);

        if (inst[text->idx].type == 'R') {
            printf("%s%s%s00000%s", (*text).s,(*text).t,(*text).d,inst[text->idx].funct);

        } else if(inst[text->idx].type == 'I') {
            printf("%s%s%s",(*text).s,(*text).t,(*text).d);

        } else if(inst[text->idx].type == 'S') {
            printf("00000%s%s%s%s",(*text).t,(*text).d,(*text).s,inst[text->idx].funct);

        } else {
            printf("%s",(*text).d);
        }
    }

    for (sym = Symbols->next; sym != NULL; sym = sym->next) {
		int d = 0;
		for(data = (*sym).first; d < (*sym).size && (*sym).first != NULL;d++)
		{
 			data = data->next;
			printf("%s",num_to_bits((*data).value,32));
		}		
    }
    printf("\n"); // To exclude "No newline at end of file"
}
