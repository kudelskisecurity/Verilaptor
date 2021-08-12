#include <iostream>
#include <verilated.h>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <fstream>

#include "Vaes_192_sed.h"

template <typename T>

void userIn(const char * var_name, T & var)
{
    unsigned tmp;
    std::cout << var_name << " = ";
    std::cin >> tmp;
    assert(tmp <= 0xffffffff);
    var = tmp;
}

void userOut(const char * var_name, unsigned value)
{
    std::cout << var_name << " = " << std::hex << value << '\n';
}

void write_result(std::unique_ptr<Vaes_192_sed>& top, std::ofstream& file) {
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[3];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[2];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[1];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[0] << '\n';;
}

void tick(std::unique_ptr<Vaes_192_sed>& top) {
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
    top->clk = 0;
}

void aes_encrypt(std::unique_ptr<Vaes_192_sed>& top) {
    tick(top);
    top->start = 0x1;
    tick(top);
    top->start = 0x0;
    tick(top);

    while (top->out_valid == 0) {
        tick(top);
    }
}

int main(int argc, char ** argv)
{
    int i;
    
    /* Initialize Verilator variables */
    Verilated::commandArgs(argc, argv);

    /* Create top module instance */
    auto top = std::make_unique<Vaes_192_sed>();

    std::cout << "[+] Simulation with Verilator\n";
    /* Inputs
    key: 8e73b0f7 da0e6452 c810f32b 809079e5 62f8ead2 522c6b7b
    plaintext:  6bc1bee2 2e409f96 e93d7e11 7393172a
    ciphertext: bd334f1d 6e45f25f f712a214 571fa5cc*/
    top->key[5] = 0x8e73b0f7;
    top->key[4] = 0xda0e6452;
    top->key[3] = 0xc810f32b;
    top->key[2] = 0x809079e5;
    top->key[1] = 0x62f8ead2;
    top->key[0] = 0x522c6b7b;

    top->state[3] = 0x6bc1bee2;
    top->state[2] = 0x2e409f96;
    top->state[1] = 0xe93d7e11;
    top->state[0] = 0x7393172a;

    std::cout << "Using key:\n";
    for (i=5;i>=0;i--) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->key[i];
    }

    std::cout << "\nUsing plaintext:\n";
    for (i=3;i>=0;i--) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->state[i];
    }

    aes_encrypt(top);

    std::cout << "\nResulting ciphertext:\n";
    for (i=3;i>=0;i--) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->out[i];
    }
    std::cout << "\n\n";
    top->final();

    return 0;
}
