#include <gtest/gtest.h>
#include <cpu.hpp>
#include <memory.hpp>
#include <memory>
#include <vector>

static void
load_and_run(std::shared_ptr<Cpu> cpu, std::shared_ptr<Memory> memory, std::vector<uint8_t> rom)
{
    memory->load(0x8000, rom);
    memory->write16(0xFFFC, 0x8000);
    cpu->reset();
    while (cpu->step()) {}
}

TEST(CpuTest, lda_a9_immediate_load_data)
{
    std::vector<uint8_t> rom       = {0xA9, 0x05, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x05);
    ASSERT_FALSE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, lda_a9_zero_flag)
{
    std::vector<uint8_t> rom       = {0xA9, 0x00, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x00);
    ASSERT_TRUE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, lda_a9_negative_flag)
{
    std::vector<uint8_t> rom       = {0xA9, 0xFD, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0xFD);
    ASSERT_FALSE(cpu->get_zero());
    ASSERT_TRUE(cpu->get_negative());
}

TEST(CpuTest, lda_from_memory)
{
    std::vector<uint8_t> rom       = {0xA5, 0x10, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    memory->write8(0x10, 0x55);
    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x55);
}

TEST(CpuTest, sta_to_memory)
{
    std::vector<uint8_t> rom       = {0xA9, 0x55, 0x85, 0x10, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(memory->read8(0x10), 0x55);
}

TEST(CpuTest, tax_aa_move_a_to_x)
{
    std::vector<uint8_t> rom       = {0xA9, 0x0A, 0xAA, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x0A);
    ASSERT_EQ(cpu->get_register_x(), 0x0A);
    ASSERT_FALSE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, tax_aa_zero_flag)
{
    std::vector<uint8_t> rom       = {0xA9, 0x00, 0xAA, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x00);
    ASSERT_EQ(cpu->get_register_x(), 0x00);
    ASSERT_TRUE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, tax_aa_negative_flag)
{
    std::vector<uint8_t> rom       = {0xA9, 0xA2, 0xAA, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0xA2);
    ASSERT_EQ(cpu->get_register_x(), 0xA2);
    ASSERT_FALSE(cpu->get_zero());
    ASSERT_TRUE(cpu->get_negative());
}

TEST(CpuTest, inx_e8_increment_x)
{
    std::vector<uint8_t> rom       = {0xE8, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_x(), 0x01);
    ASSERT_FALSE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, inx_e8_overflow_x)
{
    std::vector<uint8_t> rom       = {0xA9, 0xFF, 0xAA, 0xE8, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_x(), 0x00);
    ASSERT_TRUE(cpu->get_zero());
    ASSERT_FALSE(cpu->get_negative());
}

TEST(CpuTest, adc_add)
{
    std::vector<uint8_t> rom       = {0xA9, 0x10, 0x69, 0x05, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x15);
    ASSERT_FALSE(cpu->get_carry());
    ASSERT_FALSE(cpu->get_overflow());
}

TEST(CpuTest, adc_add_overflow)
{
    std::vector<uint8_t> rom       = {0xA9, 0xFC, 0x69, 0x05, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x01);
    ASSERT_TRUE(cpu->get_carry());
    ASSERT_FALSE(cpu->get_overflow());
}

TEST(CpuTest, sbc_substract_no_borrow)
{
    std::vector<uint8_t> rom       = {0xA9, 0x10, 0x38, 0xE9, 0x05, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x0B);
    ASSERT_TRUE(cpu->get_carry());
    ASSERT_FALSE(cpu->get_overflow());
}

TEST(CpuTest, sbc_substract_borrow)
{
    std::vector<uint8_t> rom       = {0xA9, 0x05, 0xE9, 0x10, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0xF4);
    ASSERT_FALSE(cpu->get_carry());
    ASSERT_FALSE(cpu->get_overflow());
}

TEST(CpuTest, sbc_overflow)
{
    std::vector<uint8_t> rom       = {0xA9, 0x80, 0x38, 0xE9, 0x01, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0x7F);
    ASSERT_TRUE(cpu->get_carry());
    ASSERT_TRUE(cpu->get_overflow());
}

TEST(CpuTest, sbc_substract_zero)
{
    std::vector<uint8_t> rom       = {0xA9, 0x00, 0xE9, 0x00, 0x00};
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    std::shared_ptr<Cpu> cpu       = std::make_shared<Cpu>(memory);

    load_and_run(cpu, memory, rom);

    ASSERT_EQ(cpu->get_register_a(), 0xFF);
    ASSERT_FALSE(cpu->get_carry());
    ASSERT_FALSE(cpu->get_overflow());
}
