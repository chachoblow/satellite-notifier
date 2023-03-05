#include "unity.h"
#include "led_matrix.h"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_getApplicableCoordinates_should_handleEmptyVector(void)
{
    LedMatrix ledMatrix(2, 2);
    auto coordinates = new std::vector<Coordinate<int>>{};
    auto actual = ledMatrix.getApplicableCoordinates(*coordinates);

    TEST_ASSERT_TRUE(actual.size() == 0);
}

void test_getApplicableCoordinates_should_returnInBoundCoordinatesOnly(void)
{
    LedMatrix ledMatrix(2, 2);
    auto coordinates = new std::vector<Coordinate<int>>{
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1},
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {-1, 2},
        {0, 2},
        {1, 2},
        {2, 2},
        {2, 1},
        {2, 0},
        {2, -1},
        {1, -1},
        {0, -1}};
    auto actual = ledMatrix.getApplicableCoordinates(*coordinates);
    auto expected = new std::vector<Coordinate<int>>{
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}};

    TEST_ASSERT_TRUE(actual.size() == (*expected).size());

    for (int i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQUAL_INT((*expected)[i].x, actual[i].x);
        TEST_ASSERT_EQUAL_INT((*expected)[i].y, actual[i].y);
    }
}

void test_transformCoordinates_should_handleEmptyVector(void)
{
    LedMatrix ledMatrix(2, 2);
    auto coordinates = new std::vector<Coordinate<float>>{};
    auto actual = ledMatrix.transformCoordinates(0.0f, 1.0f, 0.0f, 1.0f, *coordinates);

    TEST_ASSERT_TRUE(actual.size() == 0);
}

void test_transformCoordinates_should_transformCoordinatesWhenEvenlyDistributed(void)
{
    LedMatrix ledMatrix(3, 3);
    auto coordinates = new std::vector<Coordinate<float>>{
        {-100.0f, -100.0f},
        {-100.0f, 0.0f},
        {-100.0f, 100.0f},
        {0.0f, 100.0f},
        {100.0f, 100.0f},
        {100.0f, 0.0f},
        {100.0f, -100.0f},
        {0.0f, -100.0f},
        {0.0f, 0.0f}};
    auto actual = ledMatrix.transformCoordinates(-100.0f, 100.0f, -100.0f, 100.0f, *coordinates);
    auto expected = new std::vector<Coordinate<int>>{
        {0, 0},
        {0, 1},
        {0, 2},
        {1, 2},
        {2, 2},
        {2, 1},
        {2, 0},
        {1, 0},
        {1, 1}};

    TEST_ASSERT_TRUE(actual.size() == (*expected).size());

    for (int i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQUAL_INT((*expected)[i].x, actual[i].x);
        TEST_ASSERT_EQUAL_INT((*expected)[i].y, actual[i].y);
    }
}

void test_transformCoordinates_should_transformCoordinatesWhenCentrallyGrouped(void)
{
    LedMatrix ledMatrix(3, 3);
    auto coordinates = new std::vector<Coordinate<float>>{
        {-100.0f, -100.0f},
        {-100.0f, 0.0f},
        {-100.0f, 100.0f},
        {0.0f, 100.0f},
        {100.0f, 100.0f},
        {100.0f, 0.0f},
        {100.0f, -100.0f},
        {0.0f, -100.0f},
        {0.0f, 0.0f}};
    auto actual = ledMatrix.transformCoordinates(-1000.0f, 1000.0f, -1000.0f, 1000.0f, *coordinates);

    TEST_ASSERT_TRUE(actual.size() == 9);

    for (int i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQUAL_INT(1, actual[i].x);
        TEST_ASSERT_EQUAL_INT(1, actual[i].y);
    }
}

void test_transformCoordinates_should_transformCoordinatesOnBoundaries(void)
{
    LedMatrix ledMatrix(2, 2);
    auto coordinates = new std::vector<Coordinate<float>>{
        {0.0f, 0.0f},
        {0.000001f, 0.0f},
        {0.499999f, 0.0f},
        {0.5f, 0.0f},
        {0.500001f, 0.0f},
        {0.999999f, 0.0f},
        {1.0f, 0.0f},
    };
    auto actual = ledMatrix.transformCoordinates(0.0f, 1.0f, 0.0f, 1.0f, *coordinates);
    auto expected = new std::vector<Coordinate<int>>{
        {0, 0},
        {0, 0},
        {0, 0},
        {1, 0},
        {1, 0},
        {1, 0},
        {1, 0}};

    TEST_ASSERT_TRUE(actual.size() == (*expected).size());

    for (int i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQUAL_INT((*expected)[i].x, actual[i].x);
        TEST_ASSERT_EQUAL_INT((*expected)[i].y, actual[i].y);
    }
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_getApplicableCoordinates_should_handleEmptyVector);
    RUN_TEST(test_getApplicableCoordinates_should_returnInBoundCoordinatesOnly);
    RUN_TEST(test_transformCoordinates_should_handleEmptyVector);
    RUN_TEST(test_transformCoordinates_should_transformCoordinatesWhenEvenlyDistributed);
    RUN_TEST(test_transformCoordinates_should_transformCoordinatesWhenCentrallyGrouped);
    RUN_TEST(test_transformCoordinates_should_transformCoordinatesOnBoundaries);

    return UNITY_END();
}

/**
 * For Arduino framework
 */
void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    runUnityTests();
}
void loop() {}