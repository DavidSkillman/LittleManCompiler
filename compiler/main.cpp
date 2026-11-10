#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#define BASE 100
#define BUFFER_SIZE 256

#define TO_BASED(x) ((unsigned)(x + BASE * 10 - 1) % (BASE * 20 - 1)) - BASE * 10 + 1

enum TokenType {
	LABEL, OPCODE, INTEGER, NEWLINE
};

struct Token {
	TokenType type;
	int line = 0, column = 0;
	std::string name = "";
	unsigned short value = 0;
};

int main(int argc, char** argv) {
	std::string input, output;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (!strcmp(argv[i], "-o")) {
				++i;
				if (i >= argc) {
					break;
				}
				output = argv[i];
			}
			else if (!strcmp(argv[i], "-h")) {
				goto show_help;
			}
			else {
				input = argv[i];
			}
		}
	}
	else {
	show_help:
		printf("usage: compiler input_file -o output_file [-h]\n");
		return -1;
	}

	if (output.empty()) {
		printf("no output file specified\n");
		return 1;
	}

	std::map<std::string, int> opcodes = {
		{"hlt", 0},
		{"cob", 0},
		{"dat", 0},
		{"add", BASE * 1},
		{"sub", BASE * 2},
		{"sto", BASE * 3},
		{"sta", BASE * 3},
		{"lda", BASE * 5},
		{"bra", BASE * 6},
		{"brz", BASE * 7},
		{"brp", BASE * 8},
		{"inp", BASE * 9 + 1},
		{"out", BASE * 9 + 2},
	};

	std::map<std::string, bool> with_operand = {
		{"hlt", false},
		{"cob", false},
		{"dat", true},
		{"add", true},
		{"sub", true},
		{"sto", true},
		{"sta", true},
		{"lda", true},
		{"bra", true},
		{"brz", true},
		{"brp", true},
		{"inp", false},
		{"out", false},
	};

	std::ifstream file(input);
	if (!file.good()) {
		fputs("input file does not exist\n", stderr);
		return 1;
	}

	std::ofstream output_file(output, std::ios::binary);
	if (!output_file.good()) {
		fputs("could not open output file\n", stderr);
		return 1;
	}

	bool error = false;

	int line = 1;
	int column = 0;

	int ch = file.get();
	std::vector<Token> tokens;
	while (ch != EOF) {
		if (std::isalpha(ch)) {
			std::string buf;
			std::string lbuf;

			while (std::isalnum(ch)) {
				buf += ch;
				lbuf += std::tolower(ch);
				ch = file.get();
			}

			if (opcodes.contains(lbuf)) {
				Token token = { OPCODE, line, column, lbuf, opcodes[lbuf] };
				tokens.push_back(token);
			}
			else {
				Token token = { LABEL, line, column, buf };
				tokens.push_back(token);
			}

			column += buf.size();
		}
		else if (std::isdigit(ch) || ch == '-' || ch == '+') {
			std::string buf;

			buf += ch;
			ch = file.get();
			while (std::isdigit(ch)) {
				buf += ch;
				ch = file.get();
			}

			try {
				Token token = { INTEGER, line, column, buf, std::stoi(buf) };
				tokens.push_back(token);
			}
			catch (const std::invalid_argument& e) {
				std::cout << e.what() << "\n";
				goto syntax_error;
			}
			catch (const std::out_of_range& e) {
				std::cout << e.what() << "\n";
				goto syntax_error;
			}

			column += buf.size();
		}
		else if (std::isspace(ch)) {
			if (ch == '\n') {
				Token token = { NEWLINE, line, column };
				tokens.push_back(token);

				++line;
				column = 0;
			}
			else {
				++column;
			}
			ch = file.get();
		}
		else {
		syntax_error:
			printf("syntax error at line %d col %d\n", line, column);
			file.close();
			return 1;
		}
	}

	file.close();

	bool expected_opcode = false;
	bool expected_value = false;
	bool expected_newline = false;

	int virtual_pc = 0;
	std::map<std::string, short> labels;
	for (size_t i = 0; i < tokens.size(); i++) {
		if (expected_opcode && tokens[i].type != OPCODE) {
			printf("expected OPCODE at line %d col %d\n", tokens[i].line, tokens[i].column);
			error = true;
			break;
		}
		else if (expected_value && tokens[i].type != INTEGER && tokens[i].type != LABEL) {
			printf("expected INTEGER or LABEL at line %d col %d\n", tokens[i].line, tokens[i].column);
			error = true;
			break;
		}
		else if (expected_newline && tokens[i].type != NEWLINE) {
			printf("expected NEWLINE at line %d col %d\n", tokens[i].line, tokens[i].column);
			error = true;
			break;
		}

		switch (tokens[i].type) {
		case LABEL:
			if (expected_value) { // Usage
				expected_value = false;
				expected_newline = true;
			}
			else { // Definition
				expected_opcode = true;
				labels[tokens[i].name] = virtual_pc;
				tokens.erase(tokens.begin() + i--);
			}
			break;
		case OPCODE:
			expected_opcode = false;

			// If they aren't the I/O interrupts or HLT instruction, we expect a value
			expected_value = with_operand[tokens[i].name];
			++virtual_pc;
			break;
		case INTEGER:
			expected_value = false;
			expected_newline = true;
			break;
		case NEWLINE:
			expected_newline = false;
			tokens.erase(tokens.begin() + i--);
		}
	}

	std::vector<short> instructions;
	for (size_t i = 0; i < tokens.size(); i++) {
		short instruction = tokens[i].value;

		if (!with_operand[tokens[i].name])
			goto finish_instruction;
		i++;
		if (tokens[i].type == INTEGER) {
			instruction += tokens[i].value;
		}
		else if (tokens[i].type == LABEL) {
			if (labels.contains(tokens[i].name))
				instruction += labels[tokens[i].name];
			else {
				printf("could not find a label with the name '%s' at line %d col %d\n", tokens[i].name.c_str(), tokens[i].line, tokens[i].column);
				error = true;
			}
		}
	finish_instruction:
		instructions.push_back(TO_BASED(instruction));
	}

	if (!error) {
		output_file.write((const char*)instructions.data(), instructions.size() * sizeof(short));
		output_file.close();

		printf("written to %s\n", output.c_str());
	}
}