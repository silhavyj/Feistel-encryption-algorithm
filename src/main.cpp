#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <cstdio>

#include "cxxopts.hpp"

#define F(r,k) ((r ^ k) & ~k)
#define HIGH_BITS(x) ((x & 0xF0) >> 4)
#define LOW_BITS(x) (x & 0x0F)

#define KEY_SEPARATOR '='
#define DEBUG(msg) (arg["verbose"].as<bool>() && std::cout << msg << std::flush)

const std::string PREFIX_BIN_FILE = "feistel_";

cxxopts::ParseResult arg;
cxxopts::Options options("./feistel <input> <keys>", "KIV/BIT task 3 - feistel encryption/decryption");
std::vector<uint8_t> inputData;
std::vector<uint8_t> keys;
std::string inputFile;
std::string keyFile;

int loadInputFile(std::string fileName) {
    DEBUG("loading the content of the input file...");
    std::ifstream file;
    if (arg["binary"].as<bool>())
        file = std::ifstream(fileName, std::ios::binary);
    else
        file = std::ifstream(fileName);
    if (file.fail())
        return 1;
    inputData = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
    file.close();
    DEBUG("OK\n");
    return 0;
}

int loadKeys(std::string fileName) {
    DEBUG("loading key file...");
    std::ifstream file(fileName);
    if (file.fail())
        return 1;

    std::string line;
    while (file >> line) {
        std::size_t pos = line.find(KEY_SEPARATOR);
        if (pos == std::string::npos)
            return 2;
        std::string bin = line.substr(pos + 1, line.length());
        uint8_t num = std::stoi(bin, nullptr, 2);
        if (num > 15)
            return 3;
        keys.push_back(num);
    }
    DEBUG("OK\n");
    return 0;
}

std::vector<uint8_t> feistel(std::vector<uint8_t>& data, bool encrypt) {
    DEBUG("running feistel algorithm (");
    if (encrypt)
        DEBUG("encryption, ");
    else
        DEBUG("decryption, ");
    DEBUG("number of keys=");
    int n = (int)keys.size();
    DEBUG(n);
    DEBUG(")...");

    std::vector<uint8_t> result;
    for (uint8_t x : data) {
        uint8_t r = LOW_BITS(x);
        uint8_t l = HIGH_BITS(x);
        uint8_t ln, rn;
        uint8_t currKey;
        
        for (int i = 0; i < n; i++) {
            ln = r;
            currKey = encrypt ? keys[i] : keys[n-i-1];
            rn = F(r, currKey) ^ l;
            r = rn;
            l = ln;
        }
        result.push_back((r << 4) | l);
    }
    DEBUG("OK\n");
    return result;
}

void createBinaryFile(std::vector<uint8_t> &data) {
    DEBUG("creating final binary file...");
    std::ofstream output(PREFIX_BIN_FILE + inputFile, std::ios::binary);
    output.write((const char *)&data[0], data.size());
    output.close();
    DEBUG("OK\n");
}
void removeOutputFile() {
    DEBUG("removing output file...");
    remove(arg["output"].as<std::string>().c_str());
    DEBUG("OK\n");
}

void appendDataToOutputFile(std::vector<uint8_t> data, bool binary) {
    DEBUG("adding data into the output file...");
    std::ofstream file(arg["output"].as<std::string>(), std::ios::app);
    for (auto x : data) {
        if (binary)
            file << std::setfill('0') << std::setw(2) << std::right << std::hex << std::uppercase << (int)x << " ";
        else
            file << (char)x;
    }
    file << '\n';
    file.close();
    DEBUG("OK\n");
}

void printASCII(std::vector<uint8_t> data) {
    for (auto x : data)
        std::cout << (char)x;
    std::cout << "\n";
}

void printHEX(std::vector<uint8_t> data) {
    for (auto x : data)
        std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << std::uppercase << (int)x << " "; 
    std::cout << "\n";
}

void run() {
    auto encrypted = feistel(inputData, true);
    auto decrypted = feistel(encrypted, false);

    removeOutputFile();
    appendDataToOutputFile(encrypted, true);
    appendDataToOutputFile(decrypted, true);

    if (arg["print"].as<bool>()) {
        DEBUG("encrypted (HEX): ");   printHEX(encrypted);
        DEBUG("decrypted (HEX): ");   printHEX(decrypted);
        DEBUG("encrypted (ASCII): "); printASCII(decrypted);
    }
    
    if (arg["binary"].as<bool>()) {
        createBinaryFile(decrypted);
        std::string info = "INFO: The decrypted content of the file can be found in '" + PREFIX_BIN_FILE + inputFile + "'";
        appendDataToOutputFile(std::vector<uint8_t>(info.begin(), info.end()), false);
    } else {
        appendDataToOutputFile(decrypted, false);
    }
}

int main(int argc, char **argv) {
    options.add_options()
        ("b,binary", "the input file is a binary file", cxxopts::value<bool>()->default_value("false"))
        ("v,verbose", "print out info as the program proceeds", cxxopts::value<bool>()->default_value("false"))
        ("o,output", "name of the output file", cxxopts::value<std::string>()->default_value("output.txt"))
        ("p,print", "print out the binary data as well as the decrypted text", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "print help")
    ;
    arg = options.parse(argc, argv);
    if (arg.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    if (argc < 3) {
        std::cout << "ERR: input file or key file sis not specified!\n";
        return 1;
    }
    
    inputFile = argv[1];
    keyFile = argv[2];

    if (loadInputFile(inputFile) != 0) {
        std::cout << "input file not found!\n";
        return 1;
    }

    int res = loadKeys(keyFile);
    if (res == 1)
        std::cout << "key file not found!\n";
    else if (res == 2)
        std::cout << "key file has invalid content ('=' not found)!\n";
    else if (res == 3)
        std::cout << "key file contains a number greater than 15!\n";
    if (res != 0)
        return 1;

    run();
    return 0;
}