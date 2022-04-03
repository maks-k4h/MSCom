
#include <cstdlib>

#include "../MSCom/lib/Encoder.h"
#include "gtest/gtest.h"

class EncoderTest : public ::testing::Test {
protected:
    msc::Encoder eEmpty0;
    msc::Encoder eEmpty1;
    msc::Encoder eEmpty2;
    msc::Encoder e1Byte0;
    msc::Encoder e1Byte1;
    msc::Encoder e1Byte2;
    msc::Encoder e4Byte0;
    msc::Encoder e4Byte1;
    msc::Encoder e4Byte2;
    msc::Encoder e8Byte0;
    msc::Encoder e8Byte1;
    msc::Encoder e8Byte2;

    void SetUp() override {
        e1Byte0.setData(new uint8_t[1], 1);
        e1Byte1.setData(new uint8_t[1], 1);
        e1Byte2.setData(new uint8_t[1], 1);
        e4Byte0.setData(new uint8_t[4], 4);
        e4Byte1.setData(new uint8_t[4], 4);
        e4Byte2.setData(new uint8_t[4], 4);
        e8Byte0.setData(new uint8_t[8], 8);
        e8Byte1.setData(new uint8_t[8], 8);
        e8Byte2.setData(new uint8_t[8], 8);
    }

    void TearDown() override {
        delete[] e1Byte0.getData();
        delete[] e1Byte1.getData();
        delete[] e1Byte2.getData();
        delete[] e4Byte0.getData();
        delete[] e4Byte1.getData();
        delete[] e4Byte2.getData();
        delete[] e8Byte0.getData();
        delete[] e8Byte1.getData();
        delete[] e8Byte2.getData();
    }
};


TEST_F(EncoderTest, DefaultConstructorTest) {
    {
        EXPECT_EQ(eEmpty0.getData(), nullptr);
        EXPECT_EQ(eEmpty0.bitsInQueue(), 0);
        EXPECT_EQ(eEmpty0.bitsDataSize(), 0);
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
    }
}

TEST_F(EncoderTest, DataSetConstructorTest) {
    {
        msc::Encoder encoder(nullptr, 0);
        EXPECT_EQ(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsInQueue(), 0);
        EXPECT_EQ(encoder.bitsDataSize(), 0);
        EXPECT_EQ(encoder.bitsDone(), 0);
    }
    {
        msc::Encoder encoder(nullptr, 33);
        EXPECT_EQ(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsInQueue(), 0);
        EXPECT_EQ(encoder.bitsDataSize(), 0);
        EXPECT_EQ(encoder.bitsDone(), 0);
    }
    {
        // bits : 0110 0001  0110 0010  000 000
        auto data = new uint8_t[3];
        strcpy(reinterpret_cast<char *>(data), "ab");
        msc::Encoder encoder(data, 3);
        EXPECT_EQ(encoder.getData(), data);
        EXPECT_EQ(encoder.bitsInQueue(), 3 * 8);
        EXPECT_EQ(encoder.bitsDataSize(), 3 * 8);
        EXPECT_EQ(encoder.bitsDone(), 0);
        delete[] data;
    }
}

TEST_F(EncoderTest, CopyConstructorTest) {
    {
        msc::Encoder encoder {eEmpty0};
        EXPECT_EQ(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 0);
    }
    {
        msc::Encoder encoder {e4Byte0};
        EXPECT_NE(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 4 * 8);
    }
    {
        e8Byte0.put<8>(3);
        msc::Encoder encoder {e8Byte0};
        EXPECT_NE(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 8 * 8);
        EXPECT_EQ(encoder.bitsInQueue(), 7 * 8);
    }
}

TEST_F(EncoderTest, AssignmentOperatorTest) {
    {
        msc::Encoder encoder;
        encoder = eEmpty0;
        EXPECT_EQ(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 0);
        EXPECT_EQ(encoder.bitsInQueue(), 0);
    }
    {
        msc::Encoder encoder;
        encoder = e8Byte0;
        EXPECT_NE(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 8 * 8);
        EXPECT_EQ(encoder.bitsInQueue(), 8 * 8);
    }
    {
        msc::Encoder encoder;
        e8Byte1.put<8>(1);
        encoder = e8Byte1;
        EXPECT_NE(encoder.getData(), nullptr);
        EXPECT_EQ(encoder.bitsDataSize(), 8 * 8);
        EXPECT_EQ(encoder.bitsInQueue(), 7 * 8);
    }
}

TEST_F(EncoderTest, HasDataFunctionTest) {
    {
        EXPECT_FALSE(eEmpty0.hasData());
    }
    {
        EXPECT_TRUE(e1Byte0.hasData());
    }
    {
        EXPECT_TRUE(e8Byte0.hasData());
    }
}

TEST_F(EncoderTest, GetDataFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.getData(), nullptr);
    }
    {
        EXPECT_NE(e1Byte0.getData(), nullptr);
    }
    {
        auto *data = new uint8_t[10];
        msc::Encoder encoder(data, sizeof(data));
        EXPECT_NE(encoder.getData(), nullptr);
        delete[] data;
    }
}

TEST_F(EncoderTest, ResetBitCountFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
        eEmpty0.resetBitCount();
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
    }
    {
        e1Byte0.put<8>(48);
        EXPECT_EQ(e1Byte0.bitsDone(), 8);
        e1Byte0.resetBitCount();
        EXPECT_EQ(e1Byte0.bitsDone(), 0);
    }
    {
        e8Byte0.put<16>(18);
        EXPECT_EQ(e8Byte0.bitsDone(), 16);
        e8Byte0.resetBitCount();
        EXPECT_EQ(e8Byte0.bitsDone(), 0);
    }
}

TEST_F(EncoderTest, RoundBitCountFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
        eEmpty0.roundBitCount();
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
    }
    {
        EXPECT_EQ(e4Byte0.bitsDone(), 0);
        e4Byte0.roundBitCount();
        EXPECT_EQ(e4Byte0.bitsDone(), 0);
    }
    {
        EXPECT_EQ(e4Byte1.bitsDone(), 0);
        e4Byte1.put<1>(1);
        e4Byte1.roundBitCount();
        EXPECT_EQ(e4Byte1.bitsDone(), 8);
    }
    {
        EXPECT_EQ(e4Byte2.bitsDone(), 0);
        e4Byte2.put<8>(1);
        e4Byte2.roundBitCount();
        EXPECT_EQ(e4Byte2.bitsDone(), 8);
    }
}

TEST_F(EncoderTest, BitsInQueueFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.bitsInQueue(), 0);
    }
    {
        EXPECT_EQ(e1Byte0.bitsInQueue(), 8);
        e1Byte0.put<4>(34);
        EXPECT_EQ(e1Byte0.bitsInQueue(), 4);
        e1Byte0.put<2>(22);
        EXPECT_EQ(e1Byte0.bitsInQueue(), 2);
        e1Byte0.put<2>(22);
        EXPECT_EQ(e1Byte0.bitsInQueue(), 0);
    }
    {
        EXPECT_EQ(e8Byte0.bitsInQueue(), 64);
        e8Byte0.put<63>(1ll);
        EXPECT_EQ(e8Byte0.bitsInQueue(), 1);
        e8Byte0.put<1>(1ll);
        EXPECT_EQ(e8Byte0.bitsInQueue(), 0);
    }
}


TEST_F(EncoderTest, BitsDataSizeFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.bitsDataSize(), 0);
    }
    {
        EXPECT_EQ(e1Byte0.bitsDataSize(), 8);
    }
    {
        EXPECT_EQ(e8Byte0.bitsDataSize(), 64);
    }
    {
        std::shared_ptr<uint8_t> data(new uint8_t[10]);
        msc::Encoder encoder(data.get(), 10);
        EXPECT_EQ(encoder.bitsDataSize(), 80);
    }
}

TEST_F(EncoderTest, BitsDoneFunctionTest) {
    {
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
    }
    {
        EXPECT_EQ(e1Byte0.bitsDone(), 0);
        e1Byte0.put<4>(1);
        EXPECT_EQ(e1Byte0.bitsDone(), 4);
        e1Byte0.put<4>(1);
        EXPECT_EQ(e1Byte0.bitsDone(), 8);
    }
}

TEST_F(EncoderTest, SetDataFunctionTest) {
    {
        msc::Encoder encoder;

        std::shared_ptr<uint8_t> data0(new uint8_t[10]);
        encoder.setData(data0.get(), 10);
        EXPECT_EQ(encoder.getData(), data0.get());
        EXPECT_EQ(encoder.bitsDataSize(), 80);

        std::shared_ptr<uint8_t> data1(new uint8_t[5]);
        encoder.setData(data1.get(), 5);
        EXPECT_EQ(encoder.getData(), data1.get());
        EXPECT_EQ(encoder.bitsDataSize(), 40);
    }
}

TEST_F(EncoderTest, PutFunctionTest) {
    // put nothing
    {
        EXPECT_TRUE(eEmpty0.put<0>(1));
        EXPECT_EQ(eEmpty0.bitsDone(), 0);
        EXPECT_TRUE(e1Byte0.put<0>(1));
        EXPECT_EQ(e1Byte0.bitsDone(), 0);
    }
    // upper bounds
    {
        EXPECT_FALSE(eEmpty1.put<32>(static_cast<uint32_t>(1)));
        EXPECT_TRUE(e4Byte0.put<32>(static_cast<uint32_t>(1)));
        EXPECT_TRUE(e4Byte1.put<8>(static_cast<uint8_t>(1)));
        // e4Byte0.put<33>(static_cast<uint>(1)); // — compile time error — ok
    }
    // put by bits
    {
        e1Byte1.put<3>(0b010);
        e1Byte1.put<3>(0b010);
        e1Byte1.put<2>(0b11);
        EXPECT_EQ(*e1Byte1.getData(), 0b01001011);
        EXPECT_FALSE(e1Byte1.put<1>(1));

        e4Byte2.put<9>(0b111000111);
        EXPECT_EQ(e4Byte2.getData()[0], 0b11100011);
        EXPECT_EQ(e4Byte2.getData()[1], 0b10000000);
        EXPECT_EQ(e4Byte2.bitsDone(), 9);
    }
    // overflows
    {
        EXPECT_FALSE(eEmpty2.put<1>(1));
        EXPECT_EQ(eEmpty2.bitsDone(), 0);

        EXPECT_FALSE(e1Byte2.put<9>(1));
        EXPECT_EQ(e1Byte2.bitsDone(), 0);
        EXPECT_EQ(e1Byte2.bitsInQueue(), 8);
        EXPECT_TRUE(e1Byte2.put<5>(1));
        EXPECT_EQ(e1Byte2.bitsInQueue(), 3);
        EXPECT_FALSE(e1Byte2.put<4>(1));
        EXPECT_EQ(e1Byte2.bitsDone(), 5);
    }
}
