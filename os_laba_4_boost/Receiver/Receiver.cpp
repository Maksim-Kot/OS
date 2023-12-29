#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <boost/filesystem.hpp>
#pragma warning(disable: 4996)

namespace bip = boost::interprocess;
namespace bp = boost::process;
using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;

struct new_console : bp::extend::handler
{
	template <typename Sequence>
	void on_setup(bp::extend::windows_executor<char, Sequence>& ex)
	{
		ex.creation_flags |= CREATE_NEW_CONSOLE;
	}
};


int main()
{
	string filename;
	cout << "Input filename:\n";
	cin >> filename;
	int numOfEnters;
	cout << "Input num of enters:\n";
	cin >> numOfEnters;
	int numOfSenders;
	cout << "Input num of senders:\n";
	cin >> numOfSenders;
	ofstream out;
	out.open(filename, fstream::binary);
	out.close();

	try
	{
		bip::named_semaphore::remove("SemaphoreWrites");
		bip::named_semaphore::remove("SemaphoreReady");
		bip::named_mutex::remove("DemoMutex");
		bip::named_mutex::remove("MutexForEvent");
		bip::named_condition::remove("Event");

		// Создание и открытие именованных семафоров
		bip::named_semaphore hSemaphoreWrites(bip::create_only, "SemaphoreWrites", numOfEnters);
		bip::named_semaphore hSemaphoreReady(bip::open_or_create, "SemaphoreReady", 1 - numOfEnters);

		// Создание и открытие именованного мьютекса
		bip::named_mutex hMutex(bip::open_or_create, "DemoMutex");
		bip::named_mutex mutex(bip::open_or_create, "MutexForEvent");

		// Создание и открытие именованного условия
		bip::named_condition hEvent(bip::open_or_create, "Event");



		vector<bp::child> processes;
		string arg = "Sender.exe " + filename;


		for (int i = 0; i < numOfSenders; ++i)
		{
			bp::child c(arg, new_console());
			if (c.running()) cout << "Good\n";
			processes.push_back(std::move(c));
		}

		// Далее ваш код обработки запущенных процессов...



		vector<string> v;
		v.reserve(numOfEnters);

		// Wait for hSemaphoreReady
		hSemaphoreReady.wait();


		while (true)
		{
			cout << "Input 'read' to read file, or 'close' to stop the process:\n";
			string text;
			cin >> text;
			if (text == "close")
			{
				break;
			}
			else if (text == "read")
			{
				ifstream in;
				in.open(filename, fstream::binary);
				if (in.peek() == ifstream::traits_type::eof())
				{
					bip::scoped_lock<bip::named_mutex> lock(mutex);
					hEvent.wait(lock);
					hEvent.notify_all();
				}
				bip::scoped_lock<bip::named_mutex> lock(hMutex);
				string message;
				in.seekg(0, in.end);
				int pos = in.tellg() / 21;
				in.seekg(in.beg);
				v.clear();
				for (int i = 0; i < pos; ++i)
				{
					char mess[21];
					in.read(mess, 21);
					string s = mess;
					v.push_back(s);
				}
				in.close();
				ofstream out;
				out.open(filename, ofstream::binary | ofstream::out | ofstream::trunc);
				for (int i = 1; i < v.size(); ++i)
				{
					char mess[21];
					strcpy(mess, v[i].c_str());
					out.write(mess, 21);
				}
				out.close();
				cout << v[0] << "\n";
				hSemaphoreWrites.post();
				lock.unlock();
			}
			else cout << "Input error. Try again.\n";
		}

		for (auto& process : processes)
		{
			process.terminate();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		cout << GetLastError() << "\n";
	}

	return 0;
}