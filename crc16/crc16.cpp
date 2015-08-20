// crc16.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <direct.h>

#include "Parser.h"

extern "C"{
#include "lib_crc.h"
}

#define MAX_STRING_SIZE	2048

enum EHashType
{
	crc16,
	crckermit,
	cr16mod,
	crccittf,
	crccitt0,
	crccitt1d0f,
	crcdnp,
	crcsick,
	crc32
};

std::string getcwd()
{
	char* buffer;
	std::string path;
	// Get the current working directory: 
	if ((buffer = _getcwd(NULL, 0)) == NULL)
		perror("_getcwd error");
	else
	{
		path = std::string(buffer);
		free(buffer);
		path.append("\\");
	}

	return path;
}

void usage()
{
	std::cout << "Usage: crc16.exe [flags] -file (filename)\n" \
		"Values\n" \
		"- file 			name of file to hash(required)\n" \
		"- seed			hex seed to use - default is 0000 or the algorithm default if omitted(see below)\n" \
		"Flags\n" \
		"One(and ONLY one) of the following mode flags is required :\n" \
		"- crc16			use crc16\n" \
		"- crc32			use crc32\n" \
		"- cr16mod		use crc16 modbus with seed ffff\n" \
		"- crccittf		use crc citt with seed ffff\n" \
		"- crccitt0		use crc citt with seed 0000\n" \
		"- crccitt1d0f	use crc citt with seed 1d0f\n" \
		"- crcdnp			use crc dnp\n" \
		"- crcsick		use crc sick\n" \
		"- crckermit		use crc kermit\n" \
		"\n" \
		"The following flags are optional and mutually exclusive : \n" \
		"-hex			use hex mode(not implemented)\n" \
		"- ascii			use ascii mode(not implemented)\n" \
		"\n" \
		"Example :\n" \
		"crc16.exe - crc16 - file c:\\thepath\\thefile.txt - seed abab\n" \
		"-- this is a crc16 hash of file at c:\\thepath\\thefile.txt using seed abab hex.\n";
}

void main(int argc, char *argv[])
{
	std::map<std::string, std::pair<EArgType, bool>> argdefs;
	std::string file = "-file"; // file
	std::string seed = "-seed"; // seed
	std::string crc16 = "-crc16";
	std::string crc32 = "-crc32";
	std::string cr16mod = "-cr16mod";
	std::string crccittf = "-crccittf";
	std::string crccitt0 = "-crccitt0";
	std::string crccitt1d0f = "-crccitt1d0f";
	std::string crcdnp = "-crcdnp";
	std::string crcsick = "-crcsick";
	std::string crckermit = "-crckermit";
	std::string hex = "-hex";
	std::string ascii = "-ascii";
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(file, std::pair<EArgType, bool >(EArgType::VALUE, true)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(seed, std::pair<EArgType, bool >(EArgType::VALUE, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crc16, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crc32, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(cr16mod, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crccittf, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crccitt0, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crccitt1d0f, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crcdnp, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crcsick, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(crckermit, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(hex, std::pair<EArgType, bool>(EArgType::FLAG, false)));
	argdefs.insert(std::pair<std::string, std::pair<EArgType, bool>>(ascii, std::pair<EArgType, bool>(EArgType::FLAG, false)));

	CParser parser(argdefs);
	std::list<std::string> arglist;
	for (int i = 1; i < argc; i++)
	{
		arglist.push_back(argv[i]);
	}
	bool parsed = parser.Parse(arglist);

	if (!parsed)
	{
		usage();
		return;
	}

	std::map<std::string, std::string> opts = parser.GetOptions();

#ifdef _DEBUG
		std::cout << file << " = " << opts[file] << std::endl;
		std::cout << seed << " = " << opts[seed] << std::endl;
		std::cout << crc16 << " = " << opts[crc16] << std::endl;
		std::cout << crc32 << " = " << opts[crc32] << std::endl;
		std::cout << cr16mod << " = " << opts[cr16mod] << std::endl;
		std::cout << crccittf << " = " << opts[crccittf] << std::endl;
		std::cout << crccitt0 << " = " << opts[crccitt0] << std::endl;
		std::cout << crccitt1d0f << " = " << opts[crccitt1d0f] << std::endl;
		std::cout << crcdnp << " = " << opts[crcdnp] << std::endl;
		std::cout << crcsick << " = " << opts[crcsick] << std::endl;
		std::cout << crckermit << " = " << opts[crckermit] << std::endl;
		std::cout << hex << " = " << opts[hex] << std::endl;
		std::cout << ascii << " = " << opts[ascii] << std::endl;
#endif

	char input_string[MAX_STRING_SIZE];
	char *ptr, *dest, hex_val, prev_byte;
	unsigned short crc_16, crc_16_modbus, crc_ccitt_ffff, crc_ccitt_0000, crc_ccitt_1d0f, crc_dnp, crc_sick, crc_kermit;
	unsigned short low_byte, high_byte;
	unsigned long crc_32;
	int a, ch;
	bool do_ascii, do_hex;
	EHashType hashType = EHashType::crc16;

	crc_16 = 0;
	crc_16_modbus = 0xffff;
	crc_dnp = 0;
	crc_sick = 0;
	crc_ccitt_0000 = 0;
	crc_ccitt_ffff = 0xffff;
	crc_ccitt_1d0f = 0x1d0f;
	crc_kermit = 0;
	crc_32 = 0xffffffffL;

	if (strlen(opts[crc32].c_str()) > 0)
	{
		if (strlen(opts[seed].c_str()) > 0)
		{
			crc_32 = strtoul(opts[seed].c_str(), NULL, 16);
		}
		hashType = EHashType::crc32;
	}
	if (strlen(opts[crc16].c_str()) > 0)
	{
		if (strlen(opts[seed].c_str()) > 0)
		{
			crc_16 = strtol(opts[seed].c_str(), NULL, 16);
		}
		hashType = EHashType::crc16;
	}
	if (strlen(opts[crcdnp].c_str()) > 0)
	{
		if (strlen(opts[seed].c_str()) > 0)
		{
			crc_dnp = strtol(opts[seed].c_str(), NULL, 16);
		}
		hashType = EHashType::crcdnp;
	}
	if (strlen(opts[crcsick].c_str()) > 0)
	{
		if (strlen(opts[seed].c_str()) > 0)
		{
			crc_sick = strtol(opts[seed].c_str(), NULL, 16);
		}
		hashType = EHashType::crcsick;
	}
	if (strlen(opts[crckermit].c_str()) > 0)
	{
		if (strlen(opts[seed].c_str()) > 0)
		{
			crc_kermit = strtol(opts[seed].c_str(), NULL, 16);
		}
		hashType = EHashType::crckermit;
	}
	if (strlen(opts[cr16mod].c_str()) > 0)
		hashType = EHashType::cr16mod;
	if (strlen(opts[crccittf].c_str()) > 0)
		hashType = EHashType::cr16mod;
	if (strlen(opts[crccitt0].c_str()) > 0)
		hashType = EHashType::cr16mod;
	if (strlen(opts[crccitt1d0f].c_str()) > 0)
		hashType = EHashType::cr16mod;

	FILE *fp;

	do_ascii = (strcmp(opts[ascii].c_str(), "") == 0);
	do_hex = (strcmp(opts[hex].c_str(), "") == 0);

	std::string filename;
	if (opts[file].find(":") == std::string::npos)
	{
		filename = getcwd();
	}
	filename.append(opts[file]);

	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
	{
		std::string err = strerror(errno);
		std::cout << "Error opening file : " << err.c_str() << std::endl;
	}

	if (fp != NULL) 
	{
#ifdef _DEBUG
		std::cout << "EHashType: " << hashType << std::endl;
#endif
		prev_byte = 0;
		switch (hashType)
		{
		case EHashType::crc16:
			std::cout << "CRC 16 on " << filename.c_str() << " with seed " << std::hex << crc_16 << std::endl;
			break;
		case EHashType::crckermit:
			std::cout << "CRC KERMIT on " << filename.c_str() << " with seed " << std::hex << crc_kermit << std::endl;
			break;
		case EHashType::cr16mod:
			std::cout << "CRC 16 MODBUS on " << filename.c_str() << " with seed " << std::hex << crc_16_modbus << std::endl;
			break;
		case EHashType::crccittf:
			std::cout << "CRC CITT FFFF on " << filename.c_str() << std::endl;
			break;
		case EHashType::crccitt0:
			std::cout << "CRC CITT 0000 on " << filename.c_str() << std::endl;
			break;
		case EHashType::crccitt1d0f:
			std::cout << "CRC CITT 1D0F on " << filename.c_str() << std::endl;
			break;
		case EHashType::crcdnp:
			std::cout << "CRC DNP on " << filename.c_str() << " with seed " << std::hex << crc_dnp << std::endl;
			break;
		case EHashType::crcsick:
			std::cout << "CRC SICK on " << filename.c_str() << " with seed " << std::hex << crc_sick << std::endl;
			break;
		case EHashType::crc32:
			std::cout << "CRC 32 on " << filename.c_str() << " with seed " << std::hex << crc_32 << std::endl;
			break;
		}
		while ((ch = fgetc(fp)) != EOF)
		{
			switch (hashType)
			{
			case EHashType::crc16:
				crc_16 = update_crc_16(crc_16, (char)ch);
				break;
			case EHashType::crckermit:
				crc_kermit = update_crc_kermit(crc_kermit, (char)ch);
				break;
			case EHashType::cr16mod:
				crc_16_modbus = update_crc_16(crc_16_modbus, (char)ch);
				break;
			case EHashType::crccittf:
				crc_ccitt_ffff = update_crc_ccitt(crc_ccitt_ffff, (char)ch);
				break;
			case EHashType::crccitt0:
				crc_ccitt_0000 = update_crc_ccitt(crc_ccitt_0000, (char)ch);
				break;
			case EHashType::crccitt1d0f:
				crc_ccitt_1d0f = update_crc_ccitt(crc_ccitt_1d0f, (char)ch);
				break;
			case EHashType::crcdnp:
				crc_dnp = update_crc_dnp(crc_dnp, (char)ch);
				break;
			case EHashType::crcsick:
				crc_sick = update_crc_sick(crc_sick, (char)ch, prev_byte);
				prev_byte = (char)ch;
				break;
			case EHashType::crc32:
				crc_32 = update_crc_32(crc_32, (char)ch);
				break;
			}
		}
		fclose(fp);
	}
	switch (hashType)
	{
	case EHashType::crc16:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << crc_16 << std::endl;
		break;
	case EHashType::crckermit:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << crc_kermit << std::endl;
		break;
	case EHashType::cr16mod:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << crc_16_modbus << std::endl;
		break;
	case EHashType::crccittf:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << update_crc_ccitt << std::endl;
		break;
	case EHashType::crccitt0:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << update_crc_ccitt << std::endl;
		break;
	case EHashType::crccitt1d0f:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << update_crc_ccitt << std::endl;
		break;
	case EHashType::crcdnp:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << update_crc_dnp << std::endl;
		break;
	case EHashType::crcsick:
		std::cout << std::setw(4) << std::setfill('0') << std::hex << update_crc_sick << std::endl;
		break;
	case EHashType::crc32:
		std::cout << std::setw(8) << std::setfill('0') << std::hex << crc_32 << std::endl;
		break;
	}
}