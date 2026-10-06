/**
 * @author Tohar Markovich
 * @brief CSC 3201 Assignment 4
 */

/*includes*/
#include<atomic>
#include<fstream>
#include<iostream>
#include<mutex>
#include<vector>
#include<string>
#include<thread>

#include "bcrypt.h"

/*#defines*/

/*globals*/
std::mutex output_mutex;

/*structs*/
struct target_user {
    std::string name;
    std::string hash;
};

/*function definitions*/
void bcrypt_worker
(const std::vector<target_user>& targets, const std::vector<std::string>& word_lst, std::atomic<size_t>& global_idx, int thread_id);

/*implementations*/
void bcrypt_worker(
    const std::vector<target_user>& targets, 
    const std::vector<std::string>& word_lst, 
    std::atomic<size_t>& global_idx, 
    int thread_id)
{ using namespace std;
    size_t num_words = word_lst.size();

    while (true) {
        size_t curr_idx = global_idx.fetch_add(1);

        if (curr_idx >= num_words) break;

        const string& possible_wrd = word_lst[curr_idx];

        for (const auto& target : targets) {
            if (bcrypt::validatePassword(possible_wrd, target.hash)) {
                lock_guard<mutex> lock(output_mutex);
                cout << "Thread " << thread_id << " found password of " 
                    << target.name << " to be " << possible_wrd << "\n";
            }
        }
    }
}

int main()
{ using namespace std;
    vector<target_user> shadow_users = {
        {"Bilbo", "$2b$08$J9FW66ZdPI2nrIMcOxFYI.qx268uZn.ajhymLP/YHaAsfBGP3Fnmq"},
        {"Gandalf", "$2b$08$J9FW66ZdPI2nrIMcOxFYI.q2PW6mqALUl2/uFvV9OFNPmHGNPa6YC"},
        {"Thorin", "$2b$08$J9FW66ZdPI2nrIMcOxFYI.6B7jUcPdnqJz4tIUwKBu8lNMs5NdT9q"},
        {"Fili", "$2b$09$M9xNRFBDn0pUkPKIVCSBzuwNDDNTMWlvn7lezPr8IwVUsJbys3YZm"},
        {"Kili", "$2b$09$M9xNRFBDn0pUkPKIVCSBzuPD2bsU1q8yZPlgSdQXIBILSMCbdE4Im"},
        {"Balin", "$2b$10$xGKjb94iwmlth954hEaw3O3YmtDO/mEFLIO0a0xLK1vL79LA73Gom"},
        {"Dwalin", "$2b$10$xGKjb94iwmlth954hEaw3OFxNMF64erUqDNj6TMMKVDcsETsKK5be"},
        {"Oin", "$2b$10$xGKjb94iwmlth954hEaw3OcXR2H2PRHCgo98mjS11UIrVZLKxyABK"},
        {"Gloin", "$2b$11$/8UByex2ktrWATZOBLZ0DuAXTQl4mWX1hfSjliCvFfGH7w1tX5/3q"},
        {"Dori", "$2b$11$/8UByex2ktrWATZOBLZ0Dub5AmZeqtn7kv/3NCWBrDaRCFahGYyiq"},
        {"Nori", "$2b$11$/8UByex2ktrWATZOBLZ0DuER3Ee1GdP6f30TVIXoEhvhQDwghaU12"},
        //{"Ori", "$2b$12$rMeWZtAVcGHLEiDNeKCz8OiERmh0dh8AiNcf7ON3O3P0GWTABKh0O"},
        {"Bifur", "$2b$12$rMeWZtAVcGHLEiDNeKCz8OMoFL0k33O8Lcq33f6AznAZ/cL1LAOyK"},
        {"Bofur", "$2b$12$rMeWZtAVcGHLEiDNeKCz8Ose2KNe821.l2h5eLffzWoP01DlQb72O"},
        {"Durin", "$2b$13$6ypcazOOkUT/a7EwMuIjH.qbdqmHPDAC9B5c37RT9gEw18BX6FOay"},
    };

    // already got Ori to be airway with incorrect run

    vector<string> word_lst;
    ifstream in_file("en");
    if (!in_file.is_open()) return 1;

    string word;
#define MIN 6
#define MAX 10
    while (in_file >> word) {
        if (word.length() >= MIN && word.length() <= MAX) word_lst.push_back(word);
    }
    in_file.close();

    std::atomic<size_t> global_index(0);
    unsigned int num_threads = std::thread::hardware_concurrency();

    cout << "starting shadow cracker with " << num_threads << " parallel threads...\n";

    vector<thread> threads; 
    for (auto i{num_threads}; i-- > 0;)
        threads.push_back(thread(bcrypt_worker, cref(shadow_users), cref(word_lst), ref(global_index), num_threads-i));

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    cout << "done!\n";
}