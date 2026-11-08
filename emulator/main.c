#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#endif

#define MEMORY_SIZE 0x10000

int BASE = 100;

#define TO_BASED(x) ((unsigned)(x + BASE * 10 - 1) % (BASE * 20 - 1)) - BASE * 10 + 1

int main(int argc, char** argv) {
	char* input = NULL;
	int interactive;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (!strcmp(argv[i], "-i")) {
				interactive = 1;
			}
			else if (!strcmp(argv[i], "-b")) {
				++i;
				BASE = strtol(argv[i], NULL, 10);
			}
		}
		else {
			input = argv[i];
		}
	}

	if (!input) {
		fputs("no input file", stderr);
		exit(1);
	}

	short* memory = calloc(sizeof(short), MEMORY_SIZE);

	FILE* file = fopen(input, "rb");
    if (!file) {
        fputs("input file does not exist", stderr);
		goto end;
    }

	size_t bytesRead = fread(memory, sizeof(short), MEMORY_SIZE, file);
	fclose(file);

	short acc = 0;
	for (int i = 0; i < bytesRead; i++) {
		acc = TO_BASED(acc);
		
		int address = memory[i] % BASE;
		int read = TO_BASED(memory[address]);
		if (memory[i] < BASE) { // HLT
			break;
		}
		else if (memory[i] < BASE * 2) { // ADD
			acc += read;
		}
		else if (memory[i] < BASE * 3) { // SUB
			acc -= read;
		}
		else if (memory[i] < BASE * 4) { // STO
			memory[address] = acc;
		}
		else if (memory[i] < BASE * 5) { 
			goto bad;
		}
		else if (memory[i] < BASE * 6) { // LDA
			acc = read;
		}
		else if (memory[i] < BASE * 7) { // BRA
			i = (memory[i] % BASE) - 1;
		}
		else if (memory[i] < BASE * 8) { // BRZ
			if (acc == 0)
				i = (memory[i] % BASE) - 1;
		}
		else if (memory[i] < BASE * 9) { // BRP
			if (acc >= 0)
				i = (memory[i] % BASE) - 1;
		}
		else if (memory[i] == BASE * 9 + 1) { // INP
			if (interactive) {
#ifdef _WIN32
				acc = _getch();
#else
				scanf("%c", &acc);
#endif
			}
			else scanf("%hd", &acc);
		}
		else if (memory[i] == BASE * 9 + 2) { // OUT
			printf("%hd\n", acc);
		}
		else if (memory[i] == BASE * 9 + 22) { // OTC
			fputc(acc, stdout);
		}
		else {
		bad:
			fputs("bad instruction", stderr);
			goto end;
		}
	}

end:
	free(memory);
}