#include <gtest/gtest.h>
#include "utility/midi_encoding.h"

using namespace chordnovarw::utility;

// ── swap_int() tests ────────────────────────────────────────────

TEST(MidiEncodingTest, SwapInt1Byte) {
  EXPECT_EQ(swap_int(0x42, 1), 0x42);
}

TEST(MidiEncodingTest, SwapInt2Bytes) {
  EXPECT_EQ(swap_int(0x0102, 2), 0x0201);
  EXPECT_EQ(swap_int(0x00FF, 2), 0xFF00);
}

TEST(MidiEncodingTest, SwapInt3Bytes) {
  EXPECT_EQ(swap_int(0x010203, 3), 0x030201);
}

TEST(MidiEncodingTest, SwapInt4Bytes) {
  EXPECT_EQ(swap_int(0x01020304, 4), 0x04030201);
  // Round-trip: swap twice should give back original
  EXPECT_EQ(swap_int(swap_int(0x12345678, 4), 4), 0x12345678);
}

TEST(MidiEncodingTest, SwapIntDefault) {
  // Default len=4
  EXPECT_EQ(swap_int(0x01020304), 0x04030201);
}

// ── to_vlq() tests ─────────────────────────────────────────────

TEST(MidiEncodingTest, VlqZero) {
  auto result = to_vlq(0);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 0x00);
}

TEST(MidiEncodingTest, VlqSmallValue) {
  // Values 1-127 should be single byte
  auto result = to_vlq(1);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 0x01);

  result = to_vlq(127);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 0x7F);
}

TEST(MidiEncodingTest, VlqTwoBytes) {
  // 128 = 0x80 -> VLQ: 0x81 0x00
  auto result = to_vlq(128);
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0], 0x81);
  EXPECT_EQ(result[1], 0x00);

  // 0x3FFF (16383) -> VLQ: 0xFF 0x7F
  result = to_vlq(0x3FFF);
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0], 0xFF);
  EXPECT_EQ(result[1], 0x7F);
}

TEST(MidiEncodingTest, VlqThreeBytes) {
  // 0x4000 (16384) -> VLQ: 0x81 0x80 0x00
  auto result = to_vlq(0x4000);
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], 0x81);
  EXPECT_EQ(result[1], 0x80);
  EXPECT_EQ(result[2], 0x00);
}

TEST(MidiEncodingTest, VlqMidiStandardExample) {
  // MIDI spec example: 480 ticks per quarter = 0x01E0
  // VLQ: 0x83 0x60
  auto result = to_vlq(480);
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0], 0x83);
  EXPECT_EQ(result[1], 0x60);
}

TEST(MidiEncodingTest, VlqLargeValue) {
  // 0x0FFFFFFF -> VLQ: 0xFF 0xFF 0xFF 0x7F (maximum 4-byte VLQ)
  auto result = to_vlq(0x0FFFFFFF);
  ASSERT_EQ(result.size(), 4u);
  EXPECT_EQ(result[0], 0xFF);
  EXPECT_EQ(result[1], 0xFF);
  EXPECT_EQ(result[2], 0xFF);
  EXPECT_EQ(result[3], 0x7F);
}

TEST(MidiEncodingTest, SwapIntConstexpr) {
  // Verify constexpr evaluation
  constexpr int val = swap_int(0x0102, 2);
  static_assert(val == 0x0201, "swap_int should be constexpr");
}
