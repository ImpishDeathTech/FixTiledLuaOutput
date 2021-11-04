/**
 * fix_tilemap.cxx
 * 
 * Copyright (C) 2020-2021  Christopher Stephen Rafuse <ImpishDeathTech@protonmail.ch>
 * License : BSD-3-Clause
 * 
 * Lua Adapter doesn't seem to recognize the ["key"] values of
 * the lua tables the tilemapping application "Tiled" puts out 
 * thus, I wrote this simple little application to cull the chars
 * from the file to simplify things for myself. 
 * Or I'm missing something and ovecomplicating things for myself
 * 
 * c++ -std=c++17 -o fix-tileset fix_tileset.cxx
 *
 **/
 
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>


#define LBRACKET '['
#define RBRACKET ']'
#define DQUOTE   '"'
#define NC       '\0'
#define PROMPT   "Enter %s filename (without suffix): "
#define PATH     "resources/tilesets/%s.lua"
#define START    "Starting fix >,..,>\n\n"
#define FINISH   "Finished fix ^,..,^\n"
#define ERR_MSG  "\n[fix-tileset]->[ERROR!](%s)\n\t./fix-tileset --help\n\n"

const char* HELP_MSG = { 
R"_help_(
This program requires up to 2 parameters
usage:
	./fix-tileset <FILE PATH>                    - replaces the old file with the fixed file
	./fix-tileset <IN FILE PATH> <OUT FILE PATH> - fixes and outputs to a new file
	./fix-tileset --help                         - prints this message

)_help_" 
};

namespace imp {
	std::fstream      fs; // filestream
	std::stringstream ss; // stringstream
	
	bool open_input(char**);
	void output(int, char**);
	bool check_argument_count(int);
	void parse_and_cull();
}

int main(int ac, char** ap) {
	if (!imp::check_argument_count(ac))
		return EXIT_FAILURE;
	
	if (imp::open_input(ap)) {
		imp::parse_and_cull();
		
		imp::output(ac, ap);
	}
	return EXIT_SUCCESS;
}


/**
 * open the input
 * 
 * @param ap = argument pointer supposeldy containing filepaths
 * @returns true if opens, false if help
 **/
bool imp::open_input(char** ap) {
	if (strcmp(ap[1], "--help") == 0) {
		printf(HELP_MSG);
		return false;
	}
	else {
		printf(START);
		imp::fs.open(ap[1], std::ios::in);
	}
	return true;
}

/**
 * output the results
 * @param ac = arg count
 * @param ap = arg pointer
 **/
void imp::output(int ac, char** ap) {
	if (ac > 2) imp::fs.open(ap[2], std::ios::out);
	
	else imp::fs.open(ap[1], std::ios::out | std::ios::trunc);
	
	imp::fs << imp::ss.str();
	imp::fs.close();
	imp::fs.clear();
	imp::fs.flush();
	
	printf(FINISH);
}

/**
 * check the argument count to ensure it is the correct number of arguments
 * 
 * @param ac = argument count
 * @returns false if less than 3, true otherwise
 **/
bool imp::check_argument_count(int ac) {
	if (ac < 2) {
		fprintf(stderr, ERR_MSG, "Incorrect number of arguments");
		return false;
	}
	return true;
}

/**
 * parses the file and skips the '[', ']' and in the correct places '"' chars
 * when printing them to stdout and the output file
 **/
void imp::parse_and_cull() {
	char lastChar  { NC },
	     inputChar { NC };
	
	long long count { 0LL };
	
	while (!imp::fs.eof()) {
		imp::fs.get(inputChar);
		
		if (inputChar == LBRACKET) {
			lastChar = LBRACKET;
			continue;
		}
		if (inputChar == DQUOTE && lastChar == LBRACKET)
			continue;
		
		else if (inputChar == RBRACKET) {
			lastChar = RBRACKET;
			continue;
		}
		printf("\rChar Count: %lld", count++);
		imp::ss << inputChar;
	}
	printf("\n\n");
	
	imp::fs.close();
	imp::fs.clear();
	imp::fs.flush();
}
