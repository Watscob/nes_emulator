#include <gtest/gtest.h>
#include <cpu.hpp>
#include <vector>

TEST(CpuTest, lda_a9_immediate_load_data)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x05, 0x00};

    cpu.run(rom);

    ASSERT_EQ(cpu.register_a_, 0x05);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, lda_a9_zero_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0x00, 0x00};

    cpu.run(rom);

    ASSERT_EQ(cpu.register_a_, 0x00);
    ASSERT_TRUE(cpu.get_zero());
    ASSERT_FALSE(cpu.get_negative());
}

TEST(CpuTest, lda_a9_negative_flag)
{
    Cpu cpu;
    std::vector<std::uint8_t> rom = {0xA9, 0xFD, 0x00};

    cpu.run(rom);

    ASSERT_EQ(cpu.register_a_, 0xFD);
    ASSERT_FALSE(cpu.get_zero());
    ASSERT_TRUE(cpu.get_negative());
}
