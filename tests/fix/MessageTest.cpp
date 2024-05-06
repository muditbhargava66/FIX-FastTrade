#include "fix/Message.h"
#include "gtest/gtest.h"

using namespace FIX;

class MessageTest : public ::testing::Test {
protected:
    Message message;

    void SetUp() override {
        // Set up common fields for each test case
        message.setField(Tag::BeginString, "FIX.4.4");
        message.setField(Tag::SendingTime, "20230101-10:00:00.000");
    }
};

TEST_F(MessageTest, SetAndGetField) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::Side, "1");
    message.setField(Tag::OrderQty, "100");
    message.setField(Tag::Price, "150.50");

    EXPECT_EQ(message.getField(Tag::Symbol), "AAPL");
    EXPECT_EQ(message.getField(Tag::Side), "1");
    EXPECT_EQ(message.getField(Tag::OrderQty), "100");
    EXPECT_EQ(message.getField(Tag::Price), "150.50");
}

TEST_F(MessageTest, GetFieldWithDefaultValue) {
    EXPECT_EQ(message.getField(Tag::Symbol, "DEFAULT"), "DEFAULT");
    EXPECT_EQ(message.getField(Tag::OrderQty, "0"), "0");
}

TEST_F(MessageTest, HasField) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::OrderQty, "100");

    EXPECT_TRUE(message.hasField(Tag::Symbol));
    EXPECT_TRUE(message.hasField(Tag::OrderQty));
    EXPECT_FALSE(message.hasField(Tag::Price));
}

TEST_F(MessageTest, RemoveField) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::OrderQty, "100");

    message.removeField(Tag::OrderQty);

    EXPECT_TRUE(message.hasField(Tag::Symbol));
    EXPECT_FALSE(message.hasField(Tag::OrderQty));
}

TEST_F(MessageTest, GetFields) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::Side, "1");
    message.setField(Tag::OrderQty, "100");
    message.setField(Tag::Price, "150.50");

    const auto& fields = message.getFields();

    EXPECT_EQ(fields.size(), 6); // Including BeginString and SendingTime
    EXPECT_EQ(fields.at(Tag::BeginString), "FIX.4.4");
    EXPECT_EQ(fields.at(Tag::SendingTime), "20230101-10:00:00.000");
    EXPECT_EQ(fields.at(Tag::Symbol), "AAPL");
    EXPECT_EQ(fields.at(Tag::Side), "1");
    EXPECT_EQ(fields.at(Tag::OrderQty), "100");
    EXPECT_EQ(fields.at(Tag::Price), "150.50");
}

TEST_F(MessageTest, CopyConstructor) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::Side, "1");
    message.setField(Tag::OrderQty, "100");

    Message copiedMessage(message);

    EXPECT_EQ(copiedMessage.getFields().size(), message.getFields().size());
    EXPECT_EQ(copiedMessage.getField(Tag::Symbol), "AAPL");
    EXPECT_EQ(copiedMessage.getField(Tag::Side), "1");
    EXPECT_EQ(copiedMessage.getField(Tag::OrderQty), "100");
}

TEST_F(MessageTest, AssignmentOperator) {
    Message otherMessage;
    otherMessage.setField(Tag::Symbol, "GOOG");
    otherMessage.setField(Tag::Side, "2");
    otherMessage.setField(Tag::OrderQty, "200");

    message = otherMessage;

    EXPECT_EQ(message.getFields().size(), otherMessage.getFields().size());
    EXPECT_EQ(message.getField(Tag::Symbol), "GOOG");
    EXPECT_EQ(message.getField(Tag::Side), "2");
    EXPECT_EQ(message.getField(Tag::OrderQty), "200");
}

TEST_F(MessageTest, Clear) {
    message.setField(Tag::Symbol, "AAPL");
    message.setField(Tag::Side, "1");
    message.setField(Tag::OrderQty, "100");

    message.clear();

    EXPECT_TRUE(message.getFields().empty());
}