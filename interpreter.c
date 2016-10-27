#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "util.h"

#define TAPE_SIZE 30000

#if defined(CONCURRENCY)
#include <pthread.h>

struct threadarg {
	const char *input;
	int offset;
	uint8_t threadtape[TAPE_SIZE];
	uint8_t *ptr;
};

void task(void *param)
{
	struct threadarg *info = (struct threadarg *)param;
	uint8_t *ptr = info->ptr;
	char current_char;
	for(int i = info->offset; (current_char = info->input[i]) != '&'; ++i) {
		switch (current_char) {
		case '>':
			++ptr;
			break;
		case '<':
			--ptr;
			break;
		case '+':
			++(*ptr);
			break;
		case '-':
			--(*ptr);
			break;
		case '.':
			putchar(*ptr);
			break;
		case ',':
			*ptr = getchar();
			break;
		case '[':
			if (!(*ptr)) {
				int loop = 1;
				while (loop > 0) {
					current_char = info->input[++i];
					if (current_char == ']')
						--loop;
					else if (current_char == '[')
						++loop;
				}
			}
			break;
		case ']':
			if (*ptr) {
				int loop = 1;
				while (loop > 0) {
					current_char = info->input[--i];
					if (current_char == '[')
						--loop;
					else if (current_char == ']')
						++loop;
				}
			}
			break;
		case '*':
			printf("\n<SyntaxError>Brainf*ck doesn't support recursive thread creation.\n");
			exit(0);
		default:
			if(current_char == '\n' || current_char == ' '){
				continue;
			}else{
				printf("\n<SyntaxError>Brainf*ck doesn't support \'%c\' operation.\n", current_char);
				exit(0);
			}
		}
	}
	pthread_exit(0);
}
#endif

void interpret(const char *const input)
{
	// Initialize the tape with 30,000 zeroes.
	uint8_t tape[TAPE_SIZE] = { 0 };

	// Set the pointer to point at the left most cell of the tape.
	uint8_t *ptr = tape;

	char current_char;
	pthread_t *threadid;
	for (int i = 0; (current_char = input[i]) != '\0'; ++i) {
		switch (current_char) {
		case '>':
			++ptr;
			break;
		case '<':
			--ptr;
			break;
		case '+':
			++(*ptr);
			break;
		case '-':
			--(*ptr);
			break;
		case '.':
			putchar(*ptr);
			break;
		case ',':
			*ptr = getchar();
			break;
		case '[':
			if (!(*ptr)) {
				int loop = 1;
				while (loop > 0) {
					current_char = input[++i];
					if (current_char == ']')
						--loop;
					else if (current_char == '[')
						++loop;
				}
			}
			break;
		case ']':
			if (*ptr) {
				int loop = 1;
				while (loop > 0) {
					current_char = input[--i];
					if (current_char == '[')
						--loop;
					else if (current_char == ']')
						++loop;
				}
			}
			break;
#if defined(CONCURRENCY)
		case '*':       // create a thread
			threadid = (pthread_t *)malloc(sizeof(pthread_t));
			struct threadarg arg;
			arg.input = input;
			arg.offset = i + 1;
			memcpy(arg.threadtape, tape, TAPE_SIZE);
			arg.ptr = arg.threadtape + (ptr-tape);
			pthread_create(threadid, NULL, (void *)task, (void *)&arg);
			// jump to the next input for the main thread
			while(input[i] != '&')
				i++;
			break;
		case '|':       //wait for the thread terminate
			pthread_join(*threadid, NULL);
			break;
#endif
		default:
			if(current_char == '\n' || current_char == ' '){
				continue;
			}else{
				printf("\n<SyntaxError>Brainf*ck doesn't support \'%c\' operation.\n", current_char);
				exit(0);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2) err("Usage: interpreter <inputfile>");
	char *file_contents = read_file(argv[1]);
	if (file_contents == NULL) err("Couldn't open file");
	interpret(file_contents);
	free(file_contents);
}
