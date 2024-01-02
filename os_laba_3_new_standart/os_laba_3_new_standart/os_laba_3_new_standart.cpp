#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
std::atomic<int> count_stop(0);
int n;
int markerCount;
int remaining_threads;
std::mutex critical_mutex;
std::mutex mtx;
std::condition_variable st;
bool ready = false;
std::vector<int> thread_action;
constexpr int THREAD_DELAY = 5;

struct numsThread
{
	int* arr;
	int num;
	bool stop = false;
};

void marker(numsThread& arrF)
{
	std::unique_lock<std::mutex> lock(mtx);
	// wait for signal from main
	st.wait(lock, [] { return ready; });
	lock.unlock();
	srand(arrF.num);
	bool check = false;
	int count = 0;
	while (!check)
	{
		int temp = rand();
		temp = temp % n;
		critical_mutex.lock();
		if (arrF.arr[temp] == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY));
			arrF.arr[temp] = arrF.num;
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY));
			count += 1;
			critical_mutex.unlock();
		}
		else
		{
			std::cout << arrF.num << " " << count << " " << temp << "\n";
			critical_mutex.unlock();
			count_stop++; //increment count_stop to make a signal for main
			while (thread_action[arrF.num - 1] < 1) {
			}
			if (arrF.stop == true) check = true;
			thread_action[arrF.num - 1] = 0;
		}
	}
	for (int i = 0; i < n; i++)
	{
		if (arrF.arr[i] == arrF.num)
		{
			arrF.arr[i] = 0;
		}
	}
}


int main()
{
	int* arr;
	std::cout << "Enter size:\n";
	std::cin >> n;
	arr = new int[n];
	for (int i = 0; i < n; i++)
	{
		arr[i] = 0;
	}
	std::cout << "Enter amount of markets:\n";
	std::cin >> markerCount;
	remaining_threads = markerCount;
	thread_action.resize(markerCount, 0);
	std::vector< std::thread> workers(markerCount);
	numsThread* arrF = new numsThread[markerCount];
	bool* check = new bool[markerCount];
	for (int i = 0; i < markerCount; i++)
	{
		arrF[i].arr = arr;
		arrF[i].num = i + 1;
		workers[i] = std::thread(marker, std::ref(arrF[i]));
		check[i] = false;
	}
	{
		std::lock_guard<std::mutex> lock(mtx);
		ready = true; //make ready true to start threads
	}
	st.notify_all();

	bool quit = true;
	while (quit)
	{
		quit = false;
		int index;
		while (count_stop < remaining_threads)
		{
			//wait for all stop event
		}
		count_stop = 0; //reset stop event
		for (int i = 0; i < n; i++)
		{
			std::cout << arr[i] << " ";
		}
		std::cout << "\n";
		std::cout << "Enter the number of the thread to be completed: ";
		std::cin >> index;
		--index;
		if (index >= markerCount || index < 0)
		{
			std::cout << "Error input.\n";
		}
		else if (check[index])
		{
			std::cout << "This thread was ended.\n";
		}
		else
		{
			arrF[index].stop = true;
			thread_action[index]++;
			workers[index].join(); //end the thread
			for (int i = 0; i < n; i++)
			{
				std::cout << arr[i] << " ";
			}
			remaining_threads--;
			std::cout << "\n";
			check[index] = true;
		}

		for (int i = 0; i < markerCount; i++)
		{
			if (!check[i])
			{
				thread_action[i]++; //continue the thread
				quit = true;
			}
		}
	}

	delete[] arr;
	delete[] arrF;
	delete[] check;
	return 0;
}
