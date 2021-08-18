#include <iostream>
#include <verilated.h>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <chrono>
#include <random>
#include <fstream>

#include "Vaes_192_sed.h"
#include "Vaes_192_sed_aes_192_sed.h"
#include "Vaes_192_sed_aes_192.h"
#include "Vaes_192_sed_one_round.h"
#include "Vaes_192_sed_table_lookup.h"
#include "Vaes_192_sed_S.h"
#include "Vaes_192_sed_T.h"


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

void tick_fault_r9(std::unique_ptr<Vaes_192_sed>& top, int sbox_num, int value) {
    top->clk = 0;
    top->eval();
    // Inject fault at output of sbox
    switch (sbox_num) {
        case 0: {
            auto sbox = top->aes_192_sed->uut->r9->t0;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 1: {
            auto sbox = top->aes_192_sed->uut->r9->t1;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 2: {
            auto sbox = top->aes_192_sed->uut->r9->t2;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 3: {
            auto sbox = top->aes_192_sed->uut->r9->t3;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
    }
    top->clk = 1;
    top->eval();
    top->clk = 0;
}

void tick_fault_r10(std::unique_ptr<Vaes_192_sed>& top, int sbox_num, int value) {
    top->clk = 0;
    top->eval();
    // Inject fault at output of sbox
    switch (sbox_num) {
        case 0: {
            auto sbox = top->aes_192_sed->uut->r10->t0;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 1: {
            auto sbox = top->aes_192_sed->uut->r10->t1;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 2: {
            auto sbox = top->aes_192_sed->uut->r10->t2;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
        case 3: {
            auto sbox = top->aes_192_sed->uut->r10->t3;
            sbox->p3 = sbox->p3 ^ value;
        }
        break;
    }
    top->clk = 1;
    top->eval();
    top->clk = 0;
}

void write_result(std::unique_ptr<Vaes_192_sed>& top, std::ofstream& file, bool verbose) {
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[3];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[2];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[1];
    file << std::setw(8) << std::setfill('0') << std::hex << top->out[0] << '\n';

    if (verbose) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->out[3];
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->out[2];
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->out[1];
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->out[0] << '\n';
    }
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

void aes_encrypt_fault_r9(std::unique_ptr<Vaes_192_sed>& top, int value, int fault) {
    int i = 0;
    tick(top);
    top->start = 0x1;
    tick(top);
    top->start = 0x0;
    tick(top);

    while (top->out_valid == 0) {
        if (i < 9) {
            tick(top);
        } else {
            tick_fault_r9(top, value, fault);
        }
        i++;
    }
}

void aes_encrypt_fault_r10(std::unique_ptr<Vaes_192_sed>& top, int value, int fault) {
    int i = 0;
    tick(top);
    top->start = 0x1;
    tick(top);
    top->start = 0x0;
    tick(top);

    while (top->out_valid == 0) {
        if (i < 10) {
            tick(top);
        } else {
            tick_fault_r10(top, value, fault);
        }
        i++;
    }
}

int main(int argc, char ** argv)
{
    bool verbose = false;

    if (argc > 1) {
        if (std::string(argv[1]) == "-v") {
            verbose = true;
        }
    }

    /* Initialize Verilator variables */
    Verilated::commandArgs(argc, argv);

    /* Create top module instance */
    auto top = std::make_unique<Vaes_192_sed>();

    /* PRNG */
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); 
    std::mt19937 g1 (seed);
    int i=0;
    std::ofstream fault_file;

    std::cout << "[+] Fault simulation with Verilator\n";

    std::cout << "Using key:\n";
    for (i=5;i>=0;i--) {
        top->key[i] = g1();
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->key[i];
    }

    std::cout << "\nUsing plaintext:\n";
    for (i=4;i>=0;i--) {
        top->state[i] = g1();
        std::cout << std::setw(8) << std::setfill('0') << std::hex << top->state[i];
    }
    std::cout << "\n\n";
    
    fault_file.open("tracefile_r11");

    // Golden sample
    aes_encrypt(top);
    write_result(top, fault_file, verbose);

    // Getting the faults in round 11
    for (i=0;i<4;i++) {
        aes_encrypt_fault_r10(top, i, g1() % 256);
        write_result(top, fault_file, verbose);
        aes_encrypt_fault_r10(top, i, g1() % 256);
        write_result(top, fault_file, verbose);
    }

    fault_file.close();
    fault_file.open("tracefile_r10");

    // Golden sample
    aes_encrypt(top);
    write_result(top, fault_file, verbose);

    // Getting the faults in round 10
    for (i=0;i<4;i++) {
        aes_encrypt_fault_r9(top, i, g1() % 256);
        write_result(top, fault_file, verbose);

        aes_encrypt_fault_r9(top, i, g1() % 256);
        write_result(top, fault_file, verbose);
    }
    fault_file.close();
    top->final();

    return 0;
}
