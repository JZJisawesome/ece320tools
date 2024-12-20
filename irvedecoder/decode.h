/**
 * @brief   Code to decode RISC-V instructions
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  Copyright (C) 2023 Nick Chan\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * Based on IRVE code, which was based on code from rv32esim
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cstdint>
#include <string>

#include "common.h"

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

/**
 * @brief       RISC-V opcodes.
 * @note        If you change this, you must also change things on the Rust side.
*/
enum class Opcode : uint8_t {
    LOAD = 0b00000      , LOAD_FP = 0b00001   , CUSTOM_0 = 0b00010  , MISC_MEM = 0b00011  ,
    OP_IMM = 0b00100    , AUIPC = 0b00101     , OP_IMM_32 = 0b00110 , B48_0 = 0b00111     ,
    STORE = 0b01000     , STORE_FP = 0b01001  , CUSTOM_1 = 0b01010  , AMO = 0b01011       ,
    OP = 0b01100        , LUI = 0b01101       , OP_32 = 0b01110     , B64 = 0b01111       ,
    MADD = 0b10000      , MSUB = 0b10001      , NMSUB = 0b10010     , NMADD = 0b10011     ,
    OP_FP = 0b10100     , RESERVED_0 = 0b10101, CUSTOM_2 = 0b10110  , B48_1 = 0b10111     ,
    BRANCH = 0b11000    , JALR = 0b11001      , RESERVED_1 = 0b11010, JAL = 0b11011       ,
    SYSTEM = 0b11100    , RESERVED_3 = 0b11101, CUSTOM_3 = 0b11110  , BGE80 = 0b11111,
};

/**
 * @brief       RISC-V instruction formats
 * @note        If you change this, you must also change things on the Rust side.
*/
enum class InstFormat {
    R_TYPE = 0, I_TYPE = 1, S_TYPE = 2, B_TYPE = 3, U_TYPE = 4, J_TYPE = 5
};

/**
 * @brief       Holds the results from decoding a RISC-V instruction.
 * @note        We are NOT supporting compressed instructions.
*/
class DecodedInst {
public:

    /**
     * @brief       The constructor, decodes an instruction.
     * @param[in]   instruction The instruction to decode.
    */
    DecodedInst(Word instruction);

    DecodedInst() = default;//FIXME remove this (needed for icache)

    DecodedInst(const DecodedInst& other) = default;
    DecodedInst& operator=(const DecodedInst& other) = default;

    /**
     * @brief       Log information about the decoded instruciton.
    */
    void log() const;

    /**
     * @brief       Get the format of the decoded instruction.
     * @details     Getting the format of the instruction should be done before using other getters
     *              since the getters assert that the fields actually exist in the format of the
     *              instruction.
     * @return      The instruction format.
    */
    InstFormat get_format() const;

    Opcode get_opcode() const;
    uint8_t get_full_opcode() const;
    uint8_t get_funct3() const;
    uint8_t get_funct5() const;
    uint8_t get_funct7() const;
    uint8_t get_rd() const;
    uint8_t get_rs1() const;//Also uimm//TODO how should we expose uimm?
    uint8_t get_rs2() const;
    Word get_imm() const;

private:
    std::string disassemble() const;
    Opcode m_opcode;//Bits [6:2]
    uint8_t m_full_opcode;

    uint8_t m_funct3;
    uint8_t m_funct5;
    uint8_t m_funct7;
    uint8_t m_rd;
    uint8_t m_rs1;
    uint8_t m_rs2;
    Word m_imm_I;
    Word m_imm_S;
    Word m_imm_B;
    Word m_imm_U;
    Word m_imm_J;

    InstFormat m_format;
};
