#include <gtest/gtest.h>
#include <array>
#include <bus.hpp>
#include <cpu.hpp>
#include <memory>
#include <nes.hpp>
#include <vector>

static constexpr std::array<uint8_t, 16> DEFAULT_ROM_HEADER = {0x4E,
                                                               0x45,
                                                               0x53,
                                                               0x1A,
                                                               0x01,
                                                               0x00,
                                                               0x31,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00,
                                                               0x00};

static std::vector<uint8_t> create_sample(std::vector<uint8_t> rom)
{
    std::vector<uint8_t> raw;
    raw.append_range(DEFAULT_ROM_HEADER);
    raw.append_range(rom);
    raw.resize(0x4010);
    raw.at(0x3FFC + 0x10) = 0x00;
    raw.at(0x3FFD + 0x10) = 0x80;

    return raw;
}

static void run_nes(std::shared_ptr<Nes> nes, std::vector<uint8_t> rom)
{
    ASSERT_TRUE(nes->load_rom(create_sample(rom)));
    nes->reset();
    while (nes->step()) {}
}

TEST(CpuTest, lda_a9_immediate_load_data)
{
    std::vector<uint8_t> rom = {0xA9, 0x05, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x05);
    ASSERT_FALSE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, lda_a9_zero_flag)
{
    std::vector<uint8_t> rom = {0xA9, 0x00, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x00);
    ASSERT_TRUE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, lda_a9_negative_flag)
{
    std::vector<uint8_t> rom = {0xA9, 0xFD, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0xFD);
    ASSERT_FALSE(nes->get_cpu()->get_zero());
    ASSERT_TRUE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, lda_from_memory)
{
    std::vector<uint8_t> rom = {0xA5, 0x10, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    nes->get_bus()->write8(0x10, 0x55);
    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x55);
}

TEST(CpuTest, sta_to_memory)
{
    std::vector<uint8_t> rom = {0xA9, 0x55, 0x85, 0x10, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_bus()->read8(0x10), 0x55);
}

TEST(CpuTest, tax_aa_move_a_to_x)
{
    std::vector<uint8_t> rom = {0xA9, 0x0A, 0xAA, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x0A);
    ASSERT_EQ(nes->get_cpu()->get_register_x(), 0x0A);
    ASSERT_FALSE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, tax_aa_zero_flag)
{
    std::vector<uint8_t> rom = {0xA9, 0x00, 0xAA, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x00);
    ASSERT_EQ(nes->get_cpu()->get_register_x(), 0x00);
    ASSERT_TRUE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, tax_aa_negative_flag)
{
    std::vector<uint8_t> rom = {0xA9, 0xA2, 0xAA, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0xA2);
    ASSERT_EQ(nes->get_cpu()->get_register_x(), 0xA2);
    ASSERT_FALSE(nes->get_cpu()->get_zero());
    ASSERT_TRUE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, inx_e8_increment_x)
{
    std::vector<uint8_t> rom = {0xE8, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_x(), 0x01);
    ASSERT_FALSE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, inx_e8_overflow_x)
{
    std::vector<uint8_t> rom = {0xA9, 0xFF, 0xAA, 0xE8, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_x(), 0x00);
    ASSERT_TRUE(nes->get_cpu()->get_zero());
    ASSERT_FALSE(nes->get_cpu()->get_negative());
}

TEST(CpuTest, adc_add)
{
    std::vector<uint8_t> rom = {0xA9, 0x10, 0x69, 0x05, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x15);
    ASSERT_FALSE(nes->get_cpu()->get_carry());
    ASSERT_FALSE(nes->get_cpu()->get_overflow());
}

TEST(CpuTest, adc_add_overflow)
{
    std::vector<uint8_t> rom = {0xA9, 0xFC, 0x69, 0x05, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x01);
    ASSERT_TRUE(nes->get_cpu()->get_carry());
    ASSERT_FALSE(nes->get_cpu()->get_overflow());
}

TEST(CpuTest, sbc_substract_no_borrow)
{
    std::vector<uint8_t> rom = {0xA9, 0x10, 0x38, 0xE9, 0x05, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x0B);
    ASSERT_TRUE(nes->get_cpu()->get_carry());
    ASSERT_FALSE(nes->get_cpu()->get_overflow());
}

TEST(CpuTest, sbc_substract_borrow)
{
    std::vector<uint8_t> rom = {0xA9, 0x05, 0xE9, 0x10, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0xF4);
    ASSERT_FALSE(nes->get_cpu()->get_carry());
    ASSERT_FALSE(nes->get_cpu()->get_overflow());
}

TEST(CpuTest, sbc_overflow)
{
    std::vector<uint8_t> rom = {0xA9, 0x80, 0x38, 0xE9, 0x01, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0x7F);
    ASSERT_TRUE(nes->get_cpu()->get_carry());
    ASSERT_TRUE(nes->get_cpu()->get_overflow());
}

TEST(CpuTest, sbc_substract_zero)
{
    std::vector<uint8_t> rom = {0xA9, 0x00, 0xE9, 0x00, 0x00};
    std::shared_ptr<Nes> nes = std::make_shared<Nes>();

    run_nes(nes, rom);

    ASSERT_EQ(nes->get_cpu()->get_register_a(), 0xFF);
    ASSERT_FALSE(nes->get_cpu()->get_carry());
    ASSERT_FALSE(nes->get_cpu()->get_overflow());
}
