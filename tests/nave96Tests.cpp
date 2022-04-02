#include "gtest/gtest.h"
#include "../MSCom/lib/nave96.h"


class Nave96Tests : public ::testing::Test {
protected:
    msc::nave96 *nave;

    void SetUp() override {
        nave = new msc::nave96;
    }

    void TearDown() override {
        delete nave;
    }
};

TEST_F(Nave96Tests, emptyDataCompression) {
    {
        char *in = nullptr;
        EXPECT_NO_THROW(nave->compress(in, 0));
    }
    {
        char *in = nullptr;
        EXPECT_EQ(nave->compress(in, 0)->getBitsInQueue(), 0);
    }
    {
        char in[] = "never compress!";
        EXPECT_NO_THROW(nave->compress(in, 0));
    }
    {
        char in[] = "never compress!";
        EXPECT_EQ(nave->compress(in, 0)->getBitsInQueue(), 0);
    }
}

TEST_F(Nave96Tests, OneByteCompression) {
    {
        char *in = new char('a');
        EXPECT_NO_THROW(nave->compress(in, 1));
    }
    {
        char *in = new char('a');
        EXPECT_NE(nave->compress(in, 1)->getBitsInQueue(), 0);
    }
}

TEST_F(Nave96Tests, SameByteCompression) {
    // ---
    {
        char in[20]{};
        EXPECT_NO_THROW(nave->compress(in, 20));
    }
    {
        char in[20]{};
        EXPECT_TRUE(nave->compress(in, 20)->getBitsInQueue() < sizeof(in) * 8);
    }
    // ---
    {
        char in[2000]{};
        EXPECT_NO_THROW(nave->compress(in, 2000));
    }
    {
        char in[2000]{};
        EXPECT_TRUE(nave->compress(in, 2000)->getBitsInQueue() < sizeof(in) * 8);
    }
    // ---
    {
        char in[4096]{};
        EXPECT_NO_THROW(nave->compress(in, 4096));
    }
    {
        char in[4096]{};
        EXPECT_TRUE(nave->compress(in, 4096)->getBitsInQueue() < sizeof(in) * 8);
    }
    // ---
    {
        char in[4097]{};
        EXPECT_NO_THROW(nave->compress(in, 4097));
    }
    {
        char in[4097]{};
        EXPECT_TRUE(nave->compress(in, 4097)->getBitsInQueue() < sizeof(in) * 8);
    }
    // ---
    {
        const int N = 100000;
        char in[N]{};
        EXPECT_NO_THROW(nave->compress(in, N));
    }
    {
        const int N = 100000;
        char in[N]{};
        //std::cout << nave->compress(in,N)->getBitsInQueue()/8;
        EXPECT_TRUE(nave->compress(in, N)->getBitsInQueue() < sizeof(in) * 8);
    }
}


TEST_F(Nave96Tests, RandomBytesCompression) {
    for (int i = 0; i < 10; ++i) {
        int N = rand() % 10000 + 2000;
        char *in = new char[N];
        Encoder *e;
        EXPECT_NO_THROW(e = nave->compress(in, N));
        EXPECT_TRUE(e->getBitsInQueue() / 8 < N);
        //std::cout << e->getBitsInQueue()/8  << '\\' << N << std::endl;
        delete[] in;
        delete e;
    }
}

TEST_F(Nave96Tests, ZeroBytesDecompression) {
    // by hands
    {
        Encoder ec;
        ec.putc(0);
        char *out = new char[5];
        auto n = nave->decompress(&ec, out);
        EXPECT_EQ(n, 0);
        delete[] out;
    }

    // using compression
    {
        char a[1]{};
        auto ec = nave->compress(a, 0);
        unsigned n;
        EXPECT_NO_THROW(n = nave->decompress(ec, a));
        EXPECT_EQ(n, 0);
    }
}

TEST_F(Nave96Tests, OneByteDecompression) {
    // by hands
    {
        Encoder ec;
        ec.putc(0);
        ec.putc('a');
        char *out = new char[5];
        auto n = nave->decompress(&ec, out);
        EXPECT_EQ(n, 1);
        delete[] out;
    }

    // using compression
    {
        char a[2] = {'a'};
        auto ec = nave->compress(a, 1);
        unsigned n;
        EXPECT_NO_THROW(n = nave->decompress(ec, a));
        EXPECT_EQ(n, 1);
    }
}

TEST_F(Nave96Tests, SameByteDecompression) {
    for (int i = 1; i <= 10; ++i) {

        auto n = i * 409;
        // by hands
        {
            Encoder ec;
            ec.putc(0);
            for (int j = 0; j < n; ++j) ec.putc('a');
            char *out = new char[n * 2];
            auto ns = nave->decompress(&ec, out);
            EXPECT_EQ(ns, n);
            for (int j = 0; j < n; ++j) EXPECT_EQ(out[j], 'a');
            delete[] out;
        }

        // using compression
        {
            char a[n * 2];
            for (int j = 0; j < n; ++j) a[j] = 'a';
            auto ec = nave->compress(a, n);
            unsigned ns;
            EXPECT_NO_THROW(ns = nave->decompress(ec, a));
            EXPECT_EQ(ns, n);
            for (int j = 0; j < n; ++j) EXPECT_EQ(a[j], 'a');
        }
    }
}

TEST_F(Nave96Tests, RandomBytesDecompression) {
    for (int i = 0; i < 10; ++i) {
        int N = rand() % 30000;
        char *in = new char[N]; // initialized with trash
        for (int j = 0; j < N; ++j) in[j] = rand() % 10;
        Encoder *e;
        EXPECT_NO_THROW(e = nave->compress(in, N));
        EXPECT_TRUE(e->getBitsInQueue() / 8 < N);
        std::cout << e->getBitsInQueue() / 8 << '\\' << N << std::endl;

        char *out = new char[N * 2]; // to be sure
        auto ds = nave->decompress(e, out);
        EXPECT_EQ(ds, N);
        for (int j = 0; j < ds; ++j) {
            if (in[j] != out[j]) {
                std::cout << j << std::endl;
            }
            EXPECT_EQ(in[j], out[j]);
        }

        delete[] in;
        delete[] out;
        delete e;
    }
}