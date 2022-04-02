
#include <cstdlib>

#include "../MSCom/lib/Decoder.h"
#include "gtest/gtest.h"

class DecoderTest : public ::testing::Test {
protected:
    msc::Decoder dEmpty0;
    msc::Decoder dEmpty1;
    msc::Decoder dEmpty2;
    msc::Decoder d1Byte0;
    msc::Decoder d1Byte1;
    msc::Decoder d1Byte2;
    msc::Decoder d4Byte0;
    msc::Decoder d4Byte1;
    msc::Decoder d4Byte2;
    msc::Decoder d8Byte0;
    msc::Decoder d8Byte1;
    msc::Decoder d8Byte2;

    void SetUp() override {
        d1Byte0.setData(new uint8_t[1], 1);
        d1Byte1.setData(new uint8_t[1], 1);
        d1Byte2.setData(new uint8_t[1], 1);
        d4Byte0.setData(new uint8_t[4], 4);
        d4Byte1.setData(new uint8_t[4], 4);
        d4Byte2.setData(new uint8_t[4], 4);
        d8Byte0.setData(new uint8_t[8], 8);
        d8Byte1.setData(new uint8_t[8], 8);
        d8Byte2.setData(new uint8_t[8], 8);
    }

    void TearDown() override {
        delete[] d1Byte0.getData();
        delete[] d1Byte1.getData();
        delete[] d1Byte2.getData();
        delete[] d4Byte0.getData();
        delete[] d4Byte1.getData();
        delete[] d4Byte2.getData();
        delete[] d8Byte0.getData();
        delete[] d8Byte1.getData();
        delete[] d8Byte2.getData();
    }
};

TEST_F(DecoderTest, DefaultConstructor) {
    {
        EXPECT_FALSE(dEmpty0.hasData());
    }
}

TEST_F(DecoderTest, ReadFromEmpty) {
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty0.get<4>(v));
        EXPECT_FALSE(b);
    }
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty1.get<21>(v));
        EXPECT_FALSE(b);
    }
}

TEST_F(DecoderTest, ReadZeroBits) {
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty0.get<0>(v));
        EXPECT_TRUE(b);
    }
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d1Byte0.get<0>(v));
        EXPECT_TRUE(b);
    }
    {
        uint64_t v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d1Byte1.get<0>(v));
        EXPECT_TRUE(b);
    }
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d8Byte0.get<0>(v));
        EXPECT_TRUE(b);
    }
}

TEST_F(DecoderTest, ReadMaxBits) {
    {
        char v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty0.get<8>(v));
        EXPECT_FALSE(b);
    }
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty1.get<32>(v));
        EXPECT_FALSE(b);
    }
    {
        uint64_t v{0};
        bool b{false};
        EXPECT_NO_THROW(b = dEmpty2.get<64>(v));
        EXPECT_FALSE(b);
    }
    {
        char v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d1Byte0.get<8>(v));
        EXPECT_TRUE(b);
    }
    {
        int v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d4Byte0.get<32>(v));
        EXPECT_TRUE(b);
    }
    {
        uint64_t v{0};
        bool b{false};
        EXPECT_NO_THROW(b = d8Byte0.get<64>(v));
        EXPECT_TRUE(b);
    }
}

TEST_F(DecoderTest, JustReadTrash) {
    {
        char v{0};
        bool b{false};
        b = d1Byte0.get<8>(v);
        EXPECT_TRUE(b);
        b = d1Byte0.get<8>(v);
        EXPECT_FALSE(b);
    }
    {
        int v{0};
        bool b{false};
        b = d4Byte0.get<16>(v);
        EXPECT_TRUE(b);
        b = d4Byte0.get<8>(v);
        EXPECT_TRUE(b);
        b = d4Byte0.get<9>(v);
        EXPECT_FALSE(b);
    }
    {
        uint64_t v{0};
        bool b{false};
        b = d1Byte1.get<4>(v);
        EXPECT_TRUE(b);
        b = d1Byte1.get<2>(v);
        EXPECT_TRUE(b);
        b = d1Byte1.get<2>(v);
        EXPECT_TRUE(b);
        b = d1Byte1.get<1>(v);
        EXPECT_FALSE(b);
    }
}

TEST_F(DecoderTest, ReadHardcodedNumbers) {
    {
        // bits : 0110 0001  0110 0010  000 000
        msc::Decoder decoder(reinterpret_cast<const uint8_t *>("ab"), 3);
        int v;
        EXPECT_TRUE(decoder.get<4>(v));
        EXPECT_EQ(v, 0b0110);
        EXPECT_TRUE(decoder.get<8>(v));
        EXPECT_EQ(v, 0b00010110);
        EXPECT_TRUE(decoder.get<3>(v));
        EXPECT_EQ(v, 0b001);
        EXPECT_TRUE(decoder.get<1>(v));
        EXPECT_EQ(v, 0b0);
        EXPECT_TRUE(decoder.get<8>(v));
        EXPECT_EQ(v, 0b00000000);
    }
    {
        msc::Decoder decoder(reinterpret_cast<const uint8_t *>("."), 2);
        char v;
        EXPECT_TRUE(decoder.get<8>(v));
        EXPECT_EQ(v, '.');
    }
}

TEST_F(DecoderTest, FeedingNullptr) {
    {
        msc::Decoder decoder(nullptr, 3);
        int v;
        double r{false};
        EXPECT_NO_THROW(r = decoder.get<8>(v));
        EXPECT_FALSE(r);
        EXPECT_FALSE(decoder.hasData());
    }
    {
        msc::Decoder decoder;
        decoder.setData(nullptr, 50);
        int v;
        double r{false};
        EXPECT_NO_THROW(r = decoder.get<8>(v));
        EXPECT_FALSE(r);
        EXPECT_FALSE(decoder.hasData());
    }
}

TEST_F(DecoderTest, BitCounterInfo) {
    {
        EXPECT_EQ(dEmpty0.bitsDone(), 0);
        EXPECT_EQ(dEmpty0.bitsInQueue(), 0);
    }
    {
        EXPECT_EQ(d8Byte0.bitsDone(), 0);
        EXPECT_EQ(d8Byte0.bitsInQueue(), 64);

        int v;
        d8Byte0.get<32>(v);

        EXPECT_EQ(d8Byte0.bitsDone(), 32);
        EXPECT_EQ(d8Byte0.bitsInQueue(), 32);

        d8Byte0.get<32>(v);

        EXPECT_EQ(d8Byte0.bitsDone(), 64);
        EXPECT_EQ(d8Byte0.bitsInQueue(), 0);
    }
}

TEST_F(DecoderTest, RoundBitCount) {
    {
        EXPECT_EQ(d4Byte0.bitsDone(), 0);
        d4Byte0.roundBitCount();
        EXPECT_EQ(d4Byte0.bitsDone(), 0);
    }
    {
        int v;
        d4Byte1.get<4>(v);
        EXPECT_EQ(d4Byte1.bitsDone(), 4);
        d4Byte1.roundBitCount();
        EXPECT_EQ(d4Byte1.bitsDone(), 8);
    }
    {
        int v;
        d1Byte0.get<7>(v);
        EXPECT_EQ(d1Byte0.bitsDone(), 7);
        d1Byte0.roundBitCount();
        EXPECT_EQ(d1Byte0.bitsDone(), 8);
    }
}

TEST_F(DecoderTest, ResetBitCount) {
    {
        int v;
        d4Byte0.get<8>(v);
        EXPECT_EQ(d4Byte0.bitsDone(), 8);
        d4Byte0.resetBitCount();
        EXPECT_EQ(d4Byte0.bitsDone(), 0);
    }
    {
        msc::Decoder decoder(reinterpret_cast<const uint8_t *>("."), 1);
        char v;
        EXPECT_TRUE(decoder.get<8>(v));
        EXPECT_EQ(v, '.');
        EXPECT_FALSE(decoder.get<8>(v));
        decoder.resetBitCount();
        EXPECT_EQ(decoder.bitsDone(), 0);
        EXPECT_TRUE(decoder.get<8>(v));
        EXPECT_EQ(v, '.');
    }
}


