#include "Reporter.h"
#pragma warning(disable:4996)

int main(int argc, char* argv[]) {
	char binfile_name[50];
	char report_name[50];
	strcpy(binfile_name, argv[1]);
	strcpy(report_name, argv[2]);
	double amount = std::atof(argv[3]);
	Read_from_bin_file(binfile_name, report_name, amount);
}