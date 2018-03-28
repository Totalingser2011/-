#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "interlib.h" 

int pin[2], pout[2];
pid_t pid;

volatile int flagch = 0, flagend = 0; //чтобы сигнал не испортил переменные, они должны храниться в памяти
char *s = NULL, *stm = NULL;

void stop(int sig)
{
	signal(SIGINT, stop);
	if (flagch == 1) {  //сигнал пришел между строками во время отчистки памяти
		flagend = 1;
		return;
	}
	else {
		if (s != NULL) {
			free(s);
		}
		if (stm != NULL) {
			free(stm);
		}
	}
	close(pin[1]);
	wait (NULL);
	exit(0);
}

void myfree(char *a) 
{
	flagch = 1;
	free(a);
	a = (char*) 0;
	flagch = 0;
	if (flagend) {
		stop(SIGINT);
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		stop(1);
	}
	signal(SIGINT, stop);
	int n; //n - кол-во вопросов
	if (pipe (pin) == -1) {
		stop(SIGINT);
	}
	if (pipe (pout) == -1) {
		stop(SIGINT);
	}
	if ((pid = fork()) == 0) {
		dup2 (pin[0], 0); //переопределяем чтение и записьс сына на pipe
		dup2 (pout[1], 1); 
		close (pin[0]);
		close (pout[0]);
		close (pin[1]);
		close (pout[1]);
		execlp (argv[1], argv[1], (char*) 0); 
		kill (getppid(), 2);
	}
	close(pin[0]);
	close(pout[1]);
	ask_qnumber(pin[1]); 
	ask_subject(pin[1]);
	n = get_qnumber(pout[0]); 
	s = get_subject(pout[0]);
	printf("\n\nThe testing started\n%d Questions\nSubject: %s\n", n, s);
	myfree(s);
	
	int res = 0, rtmp; // res - кол-во правильных ответов; rtmp - текущий результат
	for (int i = 0; i < n; ++i) {
		printf("\n");
		ask_qtext(pin[1], i);
		s = get_qtext(pout[0]);
		printf("Question number %d\n%s\n", i + 1, s);
		myfree(s);
		char ctm; // считывание ответа
		int j = 0, sizel = 1; // sizel - текущий размер строки для ответа, увеличивается в процессе считывания; j - текущая позиция для записи
		s = malloc(sizel + 1);
		while ((ctm = fgetc(stdin)) != '\n') { //считывание ответа с клавиатуры
 			if (ctm == EOF) {
				stop(SIGINT);
			}
			if (j < sizel) {
				s[j] = ctm;
			}
			else {
				stm = malloc(sizel * 2 + 1); //удваиваем размер строки и записываем в stm
				for (int k = 0; k < sizel; ++k) {
					stm[k] = s[k];
				}
				myfree(s);
				flagch = 1;
				s = stm;
				stm = NULL;
				flagch = 0;
				if (flagend) {
					stop(SIGINT);
				}
				sizel *= 2;
				s[j] = ctm;
			}
			++j;
		}
		s[j] = 0;
		
		ask_result(pin[1], s, i);
		myfree(s);
		rtmp = get_result(pout[0]);
		if (rtmp) {
			printf("True\n");
			++res;
		}
		else {
			printf("False\n");
		}
	}
	printf("\nTest over\n");
	close(pin[1]);
	if (res == n) {
		printf("Well done!\n");
	}
	printf("Your score %d/%d\n", res, n);
	wait(NULL);
	return 0;
}