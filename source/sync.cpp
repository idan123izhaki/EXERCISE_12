#include <iostream>
#include <time.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <string>
#include "fixedVector.hpp"

//Global variables
FixedVector vec(10);
std::mutex m;
std::condition_variable fullCondition, emptyCondition, doneReading, beforeWriting, beforeIteration, start;

bool count2 = false, count3 = false, check2 = true,
check3 = true, afterReading2 = true, afterReading3 = true,
synchronization2 = false, synchronization3 = false;

void randomNumbers();
void writeIntoFiles(int);



int main()
{
    std::thread tRandom(randomNumbers);
    std::thread tMod2(writeIntoFiles, 2);
    std::thread tMod3(writeIntoFiles, 3);

    tRandom.join();
    tMod2.join();
    tMod3.join();

    return 0;
}


void randomNumbers()
{
    std::ofstream outputFile("randomNumbers.txt");
    srand(time(0));
    std::unique_lock<std::mutex> ul(m, std::defer_lock);
    int randomNumber;
    for (int i = 0; i < 100; i++)
    {
        ul.lock();
        fullCondition.wait(ul, [] {return vec.push_back((rand() % (100)) + 1) == 1 ? true : false; });
        outputFile << vec[vec.size() - 1] << std::endl;
        emptyCondition.notify_all();
        ul.unlock();
    }
    outputFile.close();
}

void writeIntoFiles(int numberMod)
{
    std::ofstream outputFile("mod" + std::to_string(numberMod) + ".txt");
    int vecNumber = -1;
    std::unique_lock<std::mutex> ul(m);
    while (true)
    {
        //checking if the vector is empty- timer of 3 seconds;
        if (emptyCondition.wait_for(ul, std::chrono::seconds(1), [] { return !vec.empty(); }))
        {

            start.wait(ul, [] { return !synchronization2 && !synchronization3; });
            start.notify_one();
            std::cout << "After wait 1. Thread number: " << numberMod << std::endl;
            vecNumber = vec[vec.size() - 1];
            if (numberMod == 2)
            {
                count2 = true;
                std::cout << "Reading value " << vecNumber << ". Thread number : 2!" << std::endl;
            }
            else if (numberMod == 3)
            {
                count3 = true;
                std::cout << "Reading value " << vecNumber << " .Thread number : 3!" << std::endl;
            }
            doneReading.wait(ul, [] {return count2 && count3; });
            std::cout << "After reading " << vecNumber <<  " value. Theard number : " << numberMod << std::endl;
            if (numberMod == 2)
            {
                vec.pop_back();
                check2 = afterReading2 = false;
                //After removing the last element from the vector -> the vector is not full now
                fullCondition.notify_one();
            }
            else if (numberMod == 3)
                check3 = afterReading3 = false;

            doneReading.notify_one();
            beforeWriting.wait(ul, [] {return !afterReading2 && !afterReading3; });
            std::cout << "After removing: " << vecNumber <<". Thread number: " << numberMod << std::endl;
            
            if (vecNumber % numberMod == 0)
            {
                std::cout << "Writing the number: " << vecNumber << ", into file: " << "mod" << std::to_string(numberMod) << ".txt" << std::endl;
                if (outputFile.is_open())
                    outputFile << vecNumber << std::endl;
            }
            else
                std::cout << "Thread number: " << numberMod << ". The number " << vecNumber << ", is not devide by- " << numberMod << std::endl;
            
            beforeWriting.notify_one();
            
            if (numberMod == 2) { check2 = true; synchronization2 = true; }
            else if (numberMod == 3) { check3 = true; synchronization3 = true; }

            beforeIteration.wait(ul, [] { return check2 && check3; });
            beforeIteration.notify_one();

            if (numberMod == 2) { afterReading2 = true; count2 = false; synchronization2 = false; }
            else if (numberMod == 3) { afterReading3 = true; count3 = false; synchronization3 = false;}
            
            std::cout << "Thread " << numberMod << ", is ready." << std::endl;
        }
        else
        {
            std::cout << "Thread ID: " << std::this_thread::get_id() << " -> Done with the input, the vecotr is empty more than 3 seconds..." << std::endl;
            break;
        }
    }
    outputFile.close();
}
