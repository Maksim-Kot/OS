#include "Creator.h"
#pragma warning(disable:4996)

int main(int argc, char* argv[])
{
	char binfile_name[50];
	strcpy(binfile_name, argv[1]);
	int amount = std::atoi(argv[2]);
	writing_to_bin_file(binfile_name, amount);
}