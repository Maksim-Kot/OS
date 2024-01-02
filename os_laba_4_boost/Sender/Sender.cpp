#include <iostream>
#include <fstream>
#include <string>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#pragma warning(disable: 4996)

namespace bip = boost::interprocess;
using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;

int main(int argc, char* argv[])
{
    try
    {
        bip::named_semaphore hSemaphoreWrites(bip::open_only, "SemaphoreWrites");
        bip::named_semaphore hSemaphoreReady(bip::open_only, "SemaphoreReady"); // Start with zero available
        bip::named_mutex hMutex(bip::open_only, "DemoMutex");
        bip::named_condition hEvent(bip::open_only, "Event");

        if (argc < 2)
        {
            std::cout << "Usage: " << argv[0] << " <filename>\n";
            return 1;
        }

        string s = argv[1];
        cout << s << "\n";


        ofstream out;
        out.open(s, ofstream::binary | std::ios_base::app);
        if (!out.is_open()) cout << "File not found.\n";
        hSemaphoreReady.post();
        string message;
        while (true)
        {
            bool q = true;
            cout << "Input 'send' to send your message or input 'close' to stop:\n";
            cin >> message;
            if (message == "close")
            {
                break;
            }
            else if (message == "send")
            {

                while (q) // Input message
                {
                    hSemaphoreWrites.wait();
                    cout << "Input your message (it must not be more than 20 character):\n";
                    cin >> message;
                    if (message.size() > 20)
                    {
                        cout << "The message must not be more than 20 character. Try again.\n";
                        hSemaphoreWrites.post(); // Release semaphore if input incorrect
                    }
                    else q = false;
                }

                char mess[21];
                strcpy(mess, message.c_str());

                bip::scoped_lock<bip::named_mutex> lock(hMutex);
                if (!out.is_open()) out.open(s, ofstream::binary | std::ios_base::app); // Write message to the file
                out.write(mess, 21);
                out.close();
                lock.unlock();
                hEvent.notify_all();
            }
            else cout << "Input error. Try again.\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


    return 0;
}
