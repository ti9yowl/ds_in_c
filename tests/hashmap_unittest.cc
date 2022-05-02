#include "gtest/gtest.h"
#include "../include/hashmap.h"

int hashcode(size_t x)
{
    return (int) x;
}

bool equals(size_t x, size_t y)
{
    return x == y;
}

TEST(HashmapTest, check_init)
{
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);
    EXPECT_EQ(hndl->size, 0);
}

TEST(HashmapTest, check_contains)
{
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);
    EXPECT_EQ(hm_contains(hndl, 0), false);
}

TEST(HashmapTest, check_insert_single)
{
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);

    hm_put(hndl, 0, 1);

    EXPECT_EQ(hndl->size, 1);
    EXPECT_EQ(hm_contains(hndl, 0), true);
    size_t val;
    EXPECT_EQ(hm_get(hndl, 0, &val), true);
    EXPECT_EQ(val, 1);
}

void test_insert_helper(int* keys, int* vals, int num)
{
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);

    for(int i=0; i<num; i++)
    {
        hm_put(hndl, keys[i], vals[i]);
        EXPECT_TRUE(hm_contains(hndl, keys[i]));
        EXPECT_EQ(hndl->size, i+1);
    }

    for(int i=0; i<num; i++)
    {
        size_t val;
        hm_get(hndl, keys[i], &val);
        EXPECT_EQ(vals[i], val);
    }

    EXPECT_EQ(hndl->capacity >= 2*hndl->size, true);
}
TEST(HashmapTest, check_insert_small)
{
    int keys[] = {1,2,3};
    int vals[] = {6,7,8};

    test_insert_helper(keys, vals, 3);
}

TEST(HashmapTest, check_insert_one_resize)
{
    int keys[5];
    int vals[5];

    for(int i=0; i<5; i++)
    {
        keys[i] = i;
        vals[i] = i+5;
    }

    test_insert_helper(keys, vals, 5);
}

TEST(HashmapTest, check_insert_multiple_resize)
{
    int keys[15];
    int vals[15];

    for(int i=0; i<15; i++)
    {
        keys[i] = i;
        vals[i] = i+15;
    }

    test_insert_helper(keys, vals, 15);
}

TEST(HashmapTest, check_insert_multiple_resize_large)
{
    int nums = 1000000;
    int keys[nums];
    int vals[nums];

    for(int i=0; i<nums; i++)
    {
        keys[i] = i;
        vals[i] = i+nums;
    }

    test_insert_helper(keys, vals, nums);
}

TEST(HashmapTest, check_remove_single)
{
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);

    hm_put(hndl, 0, 1);

    EXPECT_EQ(hndl->size, 1);
    EXPECT_EQ(hm_contains(hndl, 0), true);
    EXPECT_EQ(hm_remove(hndl, 0), true);
}

TEST(HashmapTest, check_remove_multiple)
{
    int nums = 10000;
    hm_h* hndl = hm_init(hashcode, equals);
    ASSERT_TRUE(hndl != NULL);

    int keys[nums];
    int vals[nums];

    for(int i=0; i<nums; i++)
    {
        keys[i] = i;
        vals[i] = i+nums;
    }

    for(int i=0; i<nums; i++)
    {
        hm_put(hndl, keys[i], vals[i]);
    }
    EXPECT_EQ(hndl->size, nums);

    for(int i=0; i<nums; i++)
    {
        EXPECT_EQ(hm_remove(hndl, keys[i]), true);
        EXPECT_EQ(hndl->size, nums-(i+1));
    }
    
    for(int i=0; i<nums; i++)
    {
        EXPECT_EQ(hm_remove(hndl, keys[i]), false);
    }
    EXPECT_EQ(hndl->capacity, HM_INIT_CAPACITY);
}

int main(int argc, char **argv)
{
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}