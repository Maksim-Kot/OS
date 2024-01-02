#pragma once
#include "..\Creator\Creator.h"
#include<iomanip>
void Read_from_bin_file(char* bin_filename, char* reportname, double amount)
{
	employee temp;

	std::ifstream in(bin_filename, std::ios_base::binary);
	std::ofstream out(reportname);

	out << std::setw(30) << " ";
	out << "Report based on " << bin_filename << "." << "\n";
	out << std::left << std::setw(20) << "Employee's num " << std::setw(20) << "Name "
		<< std::setw(20) << "Hours " << std::setw(20) << "Salary " << "\n";
	while (in.read(reinterpret_cast<char*>(&temp), sizeof(employee)))
	{
		out << std::left << std::setw(20) << temp.num << std::setw(20) << temp.name
			<< std::setw(20) << temp.hours << std::setw(20) << temp.hours * amount << "\n";
	}
}