#include <gtest/gtest.h>
#include <cpu.hpp>
#include <vector>

TEST(CpuTest, lda_a9_immediate_load_data)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x05, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0x05);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, lda_a9_zero_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x00, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0x00);
    ASSERT_TRUE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, lda_a9_negative_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0xFD, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0xFD);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_TRUE(cpu.get_negative());
}

TEST(CpuTest, tax_aa_move_a_to_x)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x0A, 0xAA, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0x0A);
    ASSERT_EQ(cpu.register_x_, 0x0A);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, tax_aa_zero_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x00, 0xAA, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0x00);
    ASSERT_EQ(cpu.register_x_, 0x00);
    ASSERT_TRUE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, tax_aa_negative_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0xA2, 0xAA, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_a_, 0xA2);
    ASSERT_EQ(cpu.register_x_, 0xA2);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_TRUE(cpu.get_negative());
}

TEST(CpuTest, inx_e8_increment_x)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xE8, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_x_, 0x01);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, inx_e8_overflow_x)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0xFF, 0xAA, 0xE8, 0x00};

    cpu.load_and_run(rom);

    ASSERT_EQ(cpu.register_x_, 0x00);
    ASSERT_TRUE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}
