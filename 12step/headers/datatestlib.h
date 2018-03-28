struct test
{
	char* subject;

	int qnumber;

	char** qtext;

	int* list_len;

	char*** qanslist;

	int* tests_len;

	void*** tests;
};

struct test* initialization();
int str_cmp(char* s1, char* s2, int l1, int l2);
int correct_order_testing (char* s, struct test* test_data, int qnumber);
int all_words_testing (char* s, struct test* test_data, int qnumber);
int correct_space_testing (char* s, struct test* test_data, int qnumber);