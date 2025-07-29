#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"  // For Tags namespace
#include "gtest/gtest.h"

using namespace fix;

class MessageTest : public ::testing::Test {
protected:
    Message message;

    void SetUp() override {
        // Set up common fields for each test case
        message.setField(Tags::BeginString, "FIX.4.4");
        message.setField(Tags::SendingTime, "20230101-10:00:00.000");
    }
};

TEST_F(MessageTest, SetAndGetField) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::Side, "1");
    message.setField(Tags::OrderQty, "100");
    message.setField(Tags::Price, "150.50");

    EXPECT_EQ(message.getField(Tags::Symbol), "AAPL");
    EXPECT_EQ(message.getField(Tags::Side), "1");
    EXPECT_EQ(message.getField(Tags::OrderQty), "100");
    EXPECT_EQ(message.getField(Tags::Price), "150.50");
}

TEST_F(MessageTest, GetFieldWithDefaultValue) {
    // Test with non-existent field (should return empty string)
    EXPECT_EQ(message.getField(Tags::Symbol), "");
    EXPECT_EQ(message.getField(Tags::OrderQty), "");
}

TEST_F(MessageTest, HasField) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::OrderQty, "100");

    EXPECT_TRUE(message.hasField(Tags::Symbol));
    EXPECT_TRUE(message.hasField(Tags::OrderQty));
    EXPECT_FALSE(message.hasField(Tags::Price));
}

TEST_F(MessageTest, RemoveField) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::OrderQty, "100");

    message.removeField(Tags::OrderQty);

    EXPECT_TRUE(message.hasField(Tags::Symbol));
    EXPECT_FALSE(message.hasField(Tags::OrderQty));
}

TEST_F(MessageTest, GetFields) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::Side, "1");
    message.setField(Tags::OrderQty, "100");
    message.setField(Tags::Price, "150.50");

    const auto& fields = message.getFields();

    EXPECT_EQ(fields.size(), 6U); // Including BeginString and SendingTime
    EXPECT_EQ(fields.at(Tags::BeginString), "FIX.4.4");
    EXPECT_EQ(fields.at(Tags::SendingTime), "20230101-10:00:00.000");
    EXPECT_EQ(fields.at(Tags::Symbol), "AAPL");
    EXPECT_EQ(fields.at(Tags::Side), "1");
    EXPECT_EQ(fields.at(Tags::OrderQty), "100");
    EXPECT_EQ(fields.at(Tags::Price), "150.50");
}

TEST_F(MessageTest, CopyConstructor) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::Side, "1");
    message.setField(Tags::OrderQty, "100");

    Message copiedMessage(message);

    EXPECT_EQ(copiedMessage.getFields().size(), message.getFields().size());
    EXPECT_EQ(copiedMessage.getField(Tags::Symbol), "AAPL");
    EXPECT_EQ(copiedMessage.getField(Tags::Side), "1");
    EXPECT_EQ(copiedMessage.getField(Tags::OrderQty), "100");
}

TEST_F(MessageTest, AssignmentOperator) {
    Message otherMessage;
    otherMessage.setField(Tags::Symbol, "GOOG");
    otherMessage.setField(Tags::Side, "2");
    otherMessage.setField(Tags::OrderQty, "200");

    message = otherMessage;

    EXPECT_EQ(message.getFields().size(), otherMessage.getFields().size());
    EXPECT_EQ(message.getField(Tags::Symbol), "GOOG");
    EXPECT_EQ(message.getField(Tags::Side), "2");
    EXPECT_EQ(message.getField(Tags::OrderQty), "200");
}

TEST_F(MessageTest, Clear) {
    message.setField(Tags::Symbol, "AAPL");
    message.setField(Tags::Side, "1");
    message.setField(Tags::OrderQty, "100");

    message.clear();

    EXPECT_TRUE(message.getFields().empty());
}