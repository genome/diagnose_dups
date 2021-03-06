#include "diagnose_dups/Signature.hpp"
#include "testing/TestBamRecords.hpp"

#include <gtest/gtest.h>

class TestSignature : public ::testing::Test {
    public:
        bam1_t *record;
        bam1_t *record2;
        TestBamRecords records;

        TestSignature()  {
            record = records.record;
            record2 = records.record3;
        }
};

TEST_F(TestSignature, _calculate_position) {
    Signature test_sig(record);
    //NOTE the coordinates in the Signature are 0-based so that's why they're different
    ASSERT_EQ(10030, test_sig._calculate_position(record));

    Signature test_sig2(record2);
    ASSERT_EQ(16302, test_sig2._calculate_position(record2));
}

TEST_F(TestSignature, _calculate_mate_position) {
    Signature test_sig(record);
    //NOTE the coordinates in the Signature are 0-based so that's why they're different
    ASSERT_EQ(10094, test_sig._calculate_mate_position(record));

    Signature test_sig2(record2);
    ASSERT_EQ(15728, test_sig2._calculate_mate_position(record2));
}

TEST_F(TestSignature, is_equal) {
    Signature test_sig;
    Signature test_sig2;
    ASSERT_TRUE(test_sig == test_sig2);

    test_sig2.tid = 1;
    ASSERT_FALSE(test_sig == test_sig2);

    test_sig2.tid = test_sig.tid;
    test_sig2.mtid = 1;
    ASSERT_FALSE(test_sig == test_sig2);

    test_sig2.mtid = test_sig.mtid;
    test_sig2.pos = 0;
    ASSERT_FALSE(test_sig == test_sig2);

    test_sig2.pos = test_sig.pos;
    test_sig2.mpos = 0;
    ASSERT_FALSE(test_sig == test_sig2);

    test_sig2.mpos = test_sig.mpos;
    test_sig2.reverse = true;
    ASSERT_FALSE(test_sig == test_sig2);

    test_sig2.reverse = test_sig.reverse;
    test_sig2.mreverse = true;
    ASSERT_FALSE(test_sig == test_sig2);
}

TEST_F(TestSignature, hash_value) {
    Signature test_sig;
    Signature test_sig2;

    test_sig.tid = 1;
    test_sig.pos = 1001;
    test_sig.mpos = 1432;
    test_sig.mtid = 1;
    test_sig.reverse = true;
    test_sig.mreverse = false;

    test_sig2.tid = 1;
    test_sig2.pos = 1001;
    test_sig2.mpos = 1432;
    test_sig2.mtid = 1;
    test_sig2.reverse = true;
    test_sig2.mreverse = false;

    //Check that the same signature values make the same hash
    ASSERT_TRUE(hash_value(test_sig) == hash_value(test_sig2));

    //Check that if we change up a position they no longer are equal
    test_sig2.mpos = 1001;
    ASSERT_FALSE(hash_value(test_sig) == hash_value(test_sig2));

    //check that the same read, but with different strand (e.g. overlapping pairs) do NOT hash the same
    test_sig.mpos = 1001;
    test_sig2.reverse = false;
    test_sig2.mreverse = true;
    ASSERT_FALSE(hash_value(test_sig) == hash_value(test_sig2));

}

TEST_F(TestSignature, less) {
    Signature test_sig;
    Signature test_sig2;

    test_sig.tid = 1;
    test_sig.pos = 1001;
    test_sig.mpos = 1432;
    test_sig.mtid = 1;
    test_sig.reverse = false;
    test_sig.mreverse = false;

    test_sig2.tid = 1;
    test_sig2.pos = 1001;
    test_sig2.mpos = 1432;
    test_sig2.mtid = 1;
    test_sig2.reverse = false;
    test_sig2.mreverse = false;
    
    ASSERT_FALSE(test_sig < test_sig2);
    
    test_sig.pos = 1000;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);

    test_sig.pos = test_sig2.pos;
    test_sig2.tid += 1;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);

    test_sig.tid = test_sig2.tid;
    test_sig2.mtid += 1;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);

    test_sig.mtid = test_sig2.mtid;
    test_sig2.mpos += 1;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);

    test_sig.mpos = test_sig2.mpos;
    test_sig2.reverse = true;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);

    test_sig2.reverse = false;
    test_sig2.mreverse = true;
    ASSERT_TRUE(test_sig < test_sig2);
    ASSERT_FALSE(test_sig2 < test_sig);
}

TEST_F(TestSignature, is_for_rightmost_read) {
    Signature test_sig;
    test_sig.tid = 1;
    test_sig.pos = 1001;
    test_sig.mpos = 1432;
    test_sig.mtid = 1;
    test_sig.reverse = false;
    test_sig.mreverse = true;
    
    ASSERT_FALSE(test_sig.is_for_rightmost_read());
    test_sig.pos = 1500;

    ASSERT_TRUE(test_sig.is_for_rightmost_read());

    test_sig.pos = 1001;
    test_sig.tid = 20;
    ASSERT_TRUE(test_sig.is_for_rightmost_read());
}

