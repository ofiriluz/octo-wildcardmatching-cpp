/**
 * @file test.cpp
 * @author ofir iluz (iluzofir@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    int iRc = 0;

    try
    {
        iRc = RUN_ALL_TESTS();
    }
    catch (std::exception ex)
    {
        perror(ex.what());
    }
    catch (...)
    {
        perror("Error occured, aborting\n");
    }

    return iRc;
}
