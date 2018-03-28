#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "datatestlib.h"


struct test* initialization()
{

	struct test* output = malloc(sizeof(struct test));

	output->qnumber = 2;

	char* subject = "Repeat after me";

	char* qtext[] = {  
		"Print 'a B c D'",
		"Print 'a B c' in any order"
	};

	int tests_len[] = {3, 2};
	void* tests1[] = {correct_space_testing, correct_order_testing, all_words_testing};
	void* tests2[] = {correct_space_testing, all_words_testing};
	void** tests[] = {
		tests1,
		tests2,
	};

	int list_len[] = {4, 3};
	char* list1[] = {"a", "B", "c", "D"};
	char* list2[] = {"a", "B", "c"};

	char** qanslist[] = { 
		list1,
		list2
	};


	int n = output->qnumber;
	output->qtext = malloc(sizeof(char**) * n);
	output->list_len = malloc(sizeof(int) * n);
	output->tests_len = malloc(sizeof(void*) * n);
	output->qanslist = malloc(sizeof(char**) * n);
	output->tests = malloc(sizeof(void**) * n);
	output->subject = malloc(strlen(subject) + 1);

	strcpy(output->subject, subject);
	for (int i = 0; i < n; ++i) {
		output->qtext[i] = malloc(strlen(qtext[i]) + 1);
		output->qanslist[i] = malloc(sizeof(char*) * list_len[i]);
		output->tests[i] = malloc(sizeof(void*) * tests_len[i]);
		strcpy(output->qtext[i], qtext[i]);
		output->list_len[i] = list_len[i];
		output->tests_len[i] = tests_len[i];
		for (int j = 0; j < list_len[i]; ++j) {
			output->qanslist[i][j] = malloc(strlen(qanslist[i][j]) + 1);
			strcpy(output->qanslist[i][j], qanslist[i][j]);
		}
		for (int j = 0; j < tests_len[i]; ++j) {
			output->tests[i][j] = tests[i][j];
		}
	}
	return output;
}

int str_cmp(char* s1, char* s2, int l1, int l2) {
	if (l1 != l2) {
		return 0;
	}
	for (int i = 0; i < l1; ++i) {
		char c1 = s1[i], c2 = s2[i];
		if (c1 == c2) continue;
		return 0;
	}
	return 1;
}

int correct_space_testing (char* s, struct test* test_data, int qnumber)
{
	int l = strlen(s), flag_first_space = 0;
	for (int i = 0; i < l + 1; ++i) {
		if (s[i] != ' ' && s[i] != 0) {
			flag_first_space = 1;
		}
		else if (flag_first_space == 0) {
			return 0;
		}
		else {
			flag_first_space = 0;
		}
	}
	return 1;
}

int correct_order_testing (char* s, struct test* test_data, int qnumber) 
{
	int l = strlen(s), flag_first_space = 0, left = -1; //выделяем слова и сравниваем их; left - левая граница слова; res - для уменьшения кол-ва слов 
	int used [test_data->list_len[qnumber]];
	for (int i = 0; i < test_data->list_len[qnumber]; ++i) {
		used[i] = 0;
	}

	for (int i = 0; i <= l + 1; ++i) {
		if (s[i] != ' ' && s[i] != 0) {
			flag_first_space = 1;
		}
		else {
			if (flag_first_space == 1) {
				int flag_not_find = 1; // нашел слово 
				int flag_true_ord = 0;
				for (int j = 0; j < test_data->list_len[qnumber]; ++j) {
					if (used[j] == 0) {
						if (str_cmp(s + left + 1, test_data->qanslist[qnumber][j], i - left - 1, strlen(test_data->qanslist[qnumber][j]))) {
							if (flag_true_ord == 1) {
								return 0;
							}
							flag_not_find = 0;
							used[j] = 1;
							break;
						}
						flag_true_ord = 1;
					}
				}
				if (flag_not_find) {
					return 0;
				}
			}
			flag_first_space = 0;
			left = i;
		}
	}
	return 1;
}

int all_words_testing (char* s, struct test* test_data, int qnumber) 
{

	int l = strlen(s), flag_first_space = 0, left = -1, res = test_data->list_len[qnumber]; //выделяем слова и сравниваем их; left - левая граница слова; res - для уменьшения кол-ва слов 
	int used [res];
	for (int i = 0; i < res; ++i) {
		used[i] = 0;
	}

	for (int i = 0; i <= l + 1; ++i) {
		if (s[i] != ' ' && s[i] != 0) {
			flag_first_space = 1;
		}
		else {
			if (flag_first_space == 1) {
				int flag_not_find = 1; // нашел слово 
				for (int j = 0; j < test_data->list_len[qnumber]; ++j) {
					if (used[j] == 0 && str_cmp(s + left + 1, test_data->qanslist[qnumber][j], i - left - 1, strlen(test_data->qanslist[qnumber][j]))) {
						--res;
						flag_not_find = 0;
						used[j] = 1;
						break;
					}
				}
				if (flag_not_find) {
					return 0;
				}
			}
			flag_first_space = 0;
			left = i;
		}
	}
	if (res == 0) { //нашли все слова
		return 1;
	}
	return 0; //найдены не все слова
}