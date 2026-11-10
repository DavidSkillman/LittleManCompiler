#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#endif

#define INTERACTIVE 0b01
#define OUTPUT_CHAR 0b10

#define TO_BASED(x) ((unsigned)(x + base * 10 - 1) % (base * 20 - 1)) - base * 10 + 1

int main(int argc, char** argv) {
	char* input = NULL;
	int flags = 0;
	int base = 100;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			/*if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interactive")) {
				interactive = 1;
			}
			if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--character-output")) {
				character_output = 1;
			}
			else if (!strcmp(argv[i], "-b")) {
				++i;
				if (i >= argc)
					fputs("no base specified", stderr);
				else base = strtoul(argv[i], NULL, 10);
			}*/
		}
		else {
			input = argv[i];
		}
	}

	if (!input) {
		fputs("no input file", stderr);
		exit(1);
	}

	FILE* file = fopen(input, "rb");
    if (!file) {
        fputs("input file does not exist", stderr);
		goto end;
    }

	fread(&base, sizeof(short), 1, file);
	fread(&flags, sizeof(short), 1, file);

	short* memory = calloc(sizeof(short), base);
	size_t bytesRead = fread(memory, sizeof(short), base, file);
	fclose(file);

	short acc = 0;
	for (int i = 0; i < bytesRead; i++) {
		acc = TO_BASED(acc);
		
		int address = memory[i] % base;
		int read = TO_BASED(memory[address]);
		if (memory[i] < base) { // HLT
			break;
		}
		else if (memory[i] < base * 2) { // ADD
			acc += read;
		}
		else if (memory[i] < base * 3) { // SUB
			acc -= read;
		}
		else if (memory[i] < base * 4) { // STO
			memory[address] = acc;
		}
		else if (memory[i] < base * 5) { 
			goto bad;
		}
		else if (memory[i] < base * 6) { // LDA
			acc = read;
		}
		else if (memory[i] < base * 7) { // BRA
			i = (memory[i] % base) - 1;
		}
		else if (memory[i] < base * 8) { // BRZ
			if (acc == 0)
				i = (memory[i] % base) - 1;
		}
		else if (memory[i] < base * 9) { // BRP
			if (acc >= 0)
				i = (memory[i] % base) - 1;
		}
		else if (memory[i] == base * 9 + 1) { // INP
			int scano = 1;
#ifdef _WIN32
			if (flags & INTERACTIVE)
				acc = _getch();
			else scano = scanf_s("%hd", &acc);
#else
			if (flags & INTERACTIVE)
				scano = scanf("%c", &acc);
			else scano = scanf("%hd", &acc);
#endif
			if (scano != 1) {
				fputs("input error or end of file", stderr);
				goto end;
			}
		}
		else if (memory[i] == base * 9 + 2) { // OUT
			if (flags & OUTPUT_CHAR)
				fputc(acc, stdout);
			else printf("%hd\n", acc);
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