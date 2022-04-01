#include "../MSCom/lib/Encoder.h"
#include "gtest/gtest.h"

class EncoderTests : public ::testing::Test {
protected:
    Encoder *ec;

    void SetUp() override {
        ec = new Encoder;
    }

    void TearDown() override {
        delete ec;
    }
};

TEST_F(EncoderTests, HoldsReadingFromEmpty) {
    char ch;
    EXPECT_FALSE(ec->getc(ch));
}

TEST_F(EncoderTests, HoldsReadingNotExistingByte) {
    ec->putbit(0);
    ec->putbit(0);
    ec->putbit(0);
    ec->putbit(0);
    ec->putbit(0);
    ec->putbit(0);
    ec->putbit(1);
    bool bit;
    ec->getbit(bit);
    char byte;
    EXPECT_FALSE(ec->getc(byte));
}

TEST_F(EncoderTests, OneByteTest) {
    char ch = 'M';
    ec->putc(ch);
    char res;
    EXPECT_TRUE(ec->getc(res));
    EXPECT_EQ(res, ch);
}

TEST_F(EncoderTests, OneBitTest) {
    ec->putbit(1);
    char out;
    EXPECT_TRUE(ec->getc(out, 1));
    EXPECT_EQ(out, char(1));
}

TEST_F(EncoderTests, FillingOneByteByBits) {
    ec->putbit(1);
    ec->putbit(1);
    ec->putbit(1);
    ec->putbit(0);
    ec->putbit(1);
    ec->putbit(1);
    ec->putbit(1);
    ec->putbit(0);

    char out;
    ec->getc(out);
    EXPECT_EQ(out, char(0b11101110));
}

TEST_F(EncoderTests, PuttingNumberOfBitsInByte) {
    {
        char byte = 0b00101011;
        ec->putc(byte, 2);
        char out;
        EXPECT_FALSE(ec->getc(out, 3));
        EXPECT_TRUE(ec->getc(out, 2));
        EXPECT_EQ(out, char(0b11));
        ec->reset();
    }
    {
        char byte = 0b00101011;
        ec->putc(byte, 3);
        char out;
        EXPECT_FALSE(ec->getc(out, 4));
        EXPECT_TRUE(ec->getc(out, 3));
        EXPECT_EQ(out, char(0b11));
        ec->reset();
    }
    {
        char byte = 0b00101011;
        ec->putc(byte, 4);
        char out;
        EXPECT_FALSE(ec->getc(out, 5));
        EXPECT_TRUE(ec->getc(out, 4));
        EXPECT_EQ(out, char(0b1011));
        ec->reset();
    }
    {
        char byte = 0b00101011;
        ec->putc('a'); // temp
        ec->putc(byte, 8);
        char out;
        ec->getc(out); // temp
        EXPECT_FALSE(ec->getc(out, 9));
        EXPECT_TRUE(ec->getc(out));
        EXPECT_EQ(out, char(0b00101011));
        ec->reset();
    }
}


TEST_F(EncoderTests, PutInt) {
    {
        int i = 0;
        ec->putint(i, 32);
        EXPECT_EQ(ec->getBitsInQueue(), 32);
        for (int j = 0; j < 32; ++j) {
            bool bit;
            EXPECT_TRUE(ec->getbit(bit));
            EXPECT_EQ(bit, 0);
        }
    }
    {
        int i = 1;
        ec->putint(i, 32);
        char byte;
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 1);
    }
    {
        int i = 0xffff;
        ec->putint(i, 32);
        char byte;
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xff);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xff);
    }
    {
        unsigned i = 0xffff1f0f;
        ec->putint((unsigned) i, 32);
        char byte;
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xff);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xff);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0x1f);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0x0f);
    }
    {
        unsigned i = 0xf1f2;
        ec->putint((unsigned) i, 16);
        ec->putint((unsigned) i, 8);
        char byte;
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xf1);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xf2);
        ec->getc(byte);
        EXPECT_EQ(byte, (char) 0xf2);
        EXPECT_FALSE(ec->getc(byte));
    }
}

TEST_F(EncoderTests, GetInt) {
    {
        ec->putbit(true);
        int res;
        EXPECT_FALSE(ec->getint(res, 2));
        EXPECT_TRUE(ec->getint(res, 1));
        EXPECT_EQ(res, 1);
    }
    {
        ec->putbit(true);
        ec->putbit(true);
        int res;
        EXPECT_TRUE(ec->getint(res, 2));
        EXPECT_EQ(res, 3);
    }
    {
        ec->putc(0xab);
        ec->putc(0xcd);
        int res;
        EXPECT_TRUE(ec->getint(res, 16));
        EXPECT_EQ(res, 0xabcd);
    }
    {
        ec->putc(0x12);
        ec->putc(0x34);
        ec->putc(0x56);
        ec->putc(0x78);
        int res;
        EXPECT_TRUE(ec->getint(res, 32));
        EXPECT_EQ(res, 0x12345678);
    }
}

