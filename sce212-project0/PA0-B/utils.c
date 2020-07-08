#include "dir_file.h"
#include "utils.h"

void open_file(char* fname, FILE** input) {
	assert(fname != NULL);

	if (access(fname, F_OK) == -1) {
		fprintf(stderr, "The '%s' file does not exists\n",  fname);
		exit(1);
	}

	*input = fopen(fname, "r");

	if (input == NULL) {
		fprintf(stderr, "The '%s' file did not open\n", fname);
		exit(1);
	}

}

// This parse_str_to_list() split string to the tokens, and put the tokens in token_list.
// The value of return is the number of tokens.

int parse_str_to_list(const char* str, char*** token_list) 
{
	int i = 0;
	int j = 0;
	int nr_tokens = 0;
	*token_list = (char**)malloc((MAX_TOKEN_NUM) * sizeof(char*));

	while (str[i] != '\0' && str[i] != '\n')
	{
		int num = 0;

		if (str[i] != '/')
		{
			for (; str[i] != '/' && str[i] != '\0' && str[i] != '\n'; i++)
			{
				num++;
			}
			*(*token_list + j) = (char*)malloc((num + 1) * sizeof(char));

			*(*(*token_list + j) + num) = '\0';

			int t_num = num;

			for (int k = 0; k < t_num; k++)
			{
				*(*(*token_list+j) + k) = str[(i - num)];
				num--;
			}
			j++;
		}
		else i++;
	}
	nr_tokens = j;

	return nr_tokens;
}

void free_token_list(char** token_list, int num_token) {
	int index;

	for (index = 0; index < num_token; index++) {
		free(token_list[index]);
	}

	free(token_list);
}

