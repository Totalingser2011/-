#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "datatestlib.h"
#include "interlib.h" 

struct test* test_data;

void stop(int sig) 
{
	for (int i = 0; i < test_data->qnumber; ++i) {
		for (int j = 0; j < test_data->list_len[i]; ++j) {
			free(test_data->qanslist[i][j]);
		}
		free(test_data->tests[i]);
		free(test_data->qanslist[i]);
		free(test_data->qtext[i]);
	}
	free(test_data->tests);
	free(test_data->tests_len);
	free(test_data->subject);
	free(test_data->qanslist);
	free(test_data->list_len);
	free(test_data->qtext);
	free(test_data);
	exit(0);
}


int main(void)
{
	test_data = initialization();
	while (waiting_for_command(0, 1, test_data) != 0);
	stop(0);
}