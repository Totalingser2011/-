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

void stop(int); 

void wr(const int des, const void* a, const int s)
{
	if (write(des, a, s) == 0) {
		stop(SIGINT);
	}
}

void rd(const int des, void* a, const int s) 
{
	if (read(des, a, s) == 0) {
		stop(SIGINT);
	}
}

char* scan (int descr) //считывание. сначала длина, потом сама строка 
{
	int lens, sh = 0;
	rd (descr, &lens, sizeof(lens));
	char *s = malloc(lens + 1); //нулевой элемент
	s[lens] = 0; //последний элемент - 0
	while (lens > 0) {
		int readed = read(descr, s + sh, lens);
		if (readed == 0) {
			free(s);
			stop(1);
		}
		sh += readed;
		lens -= readed;
	}
	return s;
}

void send_qtext(int in, int out, struct test* test_data) {
	int qnumber;
	rd(in, &qnumber, sizeof(qnumber));
	int l = strlen (test_data->qtext[qnumber]);
	wr(out, &l, sizeof(l));
	wr(out, test_data->qtext[qnumber], l);
}

void send_subject(int in, int out, struct test* test_data) {
	int l = strlen(test_data->subject);
	wr(out, &l, sizeof(l));
	wr(out, test_data->subject, l);
}

void send_result(int in, int out, struct test* test_data) {
	int qnumber;
	rd(in, &qnumber, sizeof (qnumber)); //читаем номер вопроса
	char* s = scan(in);
	int res = 1;
	for (int i = 0; i < test_data->tests_len[qnumber]; ++i) {
		int (*func) (char*, struct test*, int) = test_data->tests[qnumber][i]; //ссылке на функцию присваиваем ссылку на фн типа void*; приведение типов
		if (func(s, test_data, qnumber) == 0) {
			res = 0;
			break;
		}
	}
	wr(out, &res, sizeof(res));
	free(s); 
}

void send_qnumber(int in, int out, struct test* test_data) {
	wr(out, &test_data->qnumber, sizeof(test_data->qnumber));
}

int waiting_for_command(int in, int out, struct test* test_data) {
	int command;
	if (read(in, &command, sizeof(command)) == 0) {
		return 0;
	}
	switch (command) {
		case COM_QTEXT:
			send_qtext(in, out, test_data);
			break;
		case COM_SUBJECT:
			send_subject(in, out, test_data);
			break;
		case COM_QTEST:
			send_result(in, out, test_data);
			break;
		default:
			send_qnumber(in, out, test_data);
	}
	return 1;
}

void ask_qnumber(int in)
{
	int ask = COM_QCOUNT;
	wr(in, &ask, sizeof(ask));
}

int get_qnumber(int out)
{
	int ask;
	rd(out, &ask, sizeof(ask));
	return ask;
}

void ask_subject(int in)
{
	char* s;
	int subject = COM_SUBJECT;
	wr(in, &subject, sizeof(subject));
}

char* get_subject(int out)
{
	char* s;
	s = scan(out);
	return s;
}

char* ask_qtext(int in, int i)
{
	char* s;
	int qtext = COM_QTEXT;
	wr(in, &qtext, sizeof(qtext));
	wr(in, &i, sizeof(i));
}

char* get_qtext(int out)
{
	char* s;
	s = scan(out);
	return s;
}

void ask_result(int in, char* s, int i)
{
	int qtest = COM_QTEST;
	int len = strlen(s) + 1;
	wr(in, &qtest, sizeof(qtest));
	wr(in, &i, sizeof(i));
	wr(in, &len, sizeof(len));
	wr(in, s, len);
}

int get_result(int out)
{
	int res;
	rd(out, &res, sizeof(res));
	return res;
}