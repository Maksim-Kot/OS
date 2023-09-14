#pragma once
#include<fstream>
#include<iostream>

struct employee
{
	int num;
	char name[10];
	double hours;
};

void writing_to_bin_file(char* filename, int amount)
{
	std::ofstream out(filename, std::ios::binary);

	std::cout << filename << "\n" << amount << std::endl;
	employee temp;
	std::cout << "Enter " << amount << " employees (num, name, hours)" << std::endl;
	for (int i = 0; i < amount; i++) {
		std::cout << i + 1 << ": ";
		std::cin >> temp.num >> temp.name >> temp.hours;
		out.write((char*)&temp, sizeof(temp));
	}
}