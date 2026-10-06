/**
 * @author Tohar Markovich
 * @brief CSC 3201 Assignment 4
 */

/*includes*/
#include<algorithm>
#include<chrono>
#include<format>
#include<iostream>
#include<random>
#include<ranges>
#include<string>
#include<string_view>
#include<openssl/sha.h>
#include<unordered_map>

/*#defines*/
//#define TRUNCATED_SIZE SHA256_DIGEST_LENGTH >> 2
#define TRUNCATED_SIZE 8
#define MAX 50

/*function definitions*/
std::string sha256(std::string_view str);
std::string trunc_hash(std::string_view str, size_t);
bool hamming_dist_one(std::string_view str1, std::string_view str2);
std::string generate_random_str(size_t);

/*implementations*/
std::string sha256(std::string_view str)
{ using namespace std;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256; SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.data(), str.size());
    SHA256_Final(hash, &sha256);

    string result;
    result.reserve(2*SHA256_DIGEST_LENGTH);

    for (auto i{0}; i < SHA256_DIGEST_LENGTH; i++)
        result += format("{:02x}", hash[i]);

    return result;
}

std::string trunc_hash(std::string_view str, size_t trunc_len)
{ using namespace std;
    string hash = sha256(str);
    size_t hex_chars = (trunc_len + 3) / 4;
    string key = hash.substr(0, hex_chars);
    if (trunc_len % 4) {
        int nib = (key.back() >= 'a') ? key.back() - 'a' + 10 : key.back() - '0';
        nib &= (0xF << (4 - trunc_len % 4)) & 0xF;
        key.back() = "0123456789abcdef"[nib];
    }
    return key;
}

bool hamming_dist_one(std::string_view str1, std::string_view str2)
{ using namespace std;
    if (str1.size() != str2.size()) return false;
    auto differences = ranges::count_if(
        views::zip(str1, str2),
        [](const auto& pair) { return get<0>(pair) != get<1>(pair); }
    );
    return differences == 1;
}

std::string generate_random_str(size_t trunc_len)
{ using namespace std;
    random_device rd; static mt19937 gen(rd());
#define ASCII_BEGIN 32
#define ASCII_END 126
    uniform_int_distribution<int> distr(ASCII_BEGIN, ASCII_END);

    string random_str;
    random_str.reserve(trunc_len);
    for (auto i{trunc_len}; i-- > 0;) 
        random_str += static_cast<char>(distr(gen));

    return random_str;
}

int main()
{ using namespace std;
    /*hamming distance test*/
    /*string hamming_str1a = "teststring";
    string hamming_str1b = "teststrXng";
    cout << "test 1: " << hamming_dist_one(hamming_str1a, hamming_str1b) << "\n";

    string hamming_str2a = "hannah";
    string hamming_str2b = "hannXh";
    cout << "test 2: " << hamming_dist_one(hamming_str2a, hamming_str2b) << "\n";

    string hamming_str3a = "shastring";
    string hamming_str3b = "shXstring";
    cout << "test 3: " << hamming_dist_one(hamming_str3a, hamming_str3b) << "\n";*/

    /*hash collision*/
    unordered_map<string, string> seen_hashes;

    /**
     * plan: 
     * 1. generate random string input
     * 2. calculate truncated hash for that string input
     * 3. check if that hash already has an associated string in the map
     * 4. if not, add it
     * 5. if so, collision found
     * note: need to calculate collision time and size of map post collision
     */


    for (auto i{TRUNCATED_SIZE}; i <= MAX; i+=2) {

    /*collision logic here*/
        seen_hashes.clear();
        bool no_collision = true;
        auto start = chrono::steady_clock::now();
        while(no_collision) {
            string test = generate_random_str(i);
            string hash = trunc_hash(test, i);
            if (seen_hashes.count(hash)) {
               /* string first_str = seen_hashes[hash];
                cout << "collision\n";
                cout << "first string: " << first_str << " with hash" << hash << "\n";
                cout << "second string: " << test << " with hash" << hash << "\n";*/
                no_collision = false;
            }
            else {
                seen_hashes[hash] = test;
            }
        }
        auto end = chrono::steady_clock::now();
        auto elapsed = end - start;
        auto mis = chrono::duration_cast<chrono::microseconds>(elapsed).count();
        cout << mis << "\n";

        //cout << seen_hashes.size() << "\n";
    }

  //  auto end = chrono::steady_clock::now();

   // auto elapsed = end - start;
    
    //auto s = chrono::duration_cast<chrono::seconds>(elapsed).count();
    /*just in case, deterine ms and m too*/
   /* auto m = chrono::duration_cast<chrono::minutes>(elapsed).count();
   */// auto ms = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

    //cout << "elapsed time (ms) until collision: " << ms << "\n";

    cout << "map size post collision: " << seen_hashes.size() << "\n";
}