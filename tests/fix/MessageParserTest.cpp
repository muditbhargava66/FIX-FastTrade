#include "fix/MessageParser.h"
#include "fix/ZeroCopyMessage.h"  // For Tags namespace
#include "gtest/gtest.h"

using namespace fix;

TEST(MessageParserTest, ParseValidMessage) {
    // Arrange
    std::string messageString = "8=FIX.4.2|9=145|35=D|49=SENDER|56=TARGET|34=1|52=20230101-12:30:00|11=ORDER123|21=1|55=SYMBOL|54=1|60=20230101-12:30:00|38=100|40=2|44=50.00|10=123|";

    // Act
    Message message = MessageParser::parse(messageString);

    // Assert
    EXPECT_EQ(message.getField(Tags::BeginString), "FIX.4.2");
    EXPECT_EQ(message.getField(Tags::BodyLength), "145");
    EXPECT_EQ(message.getField(Tags::MsgType), "D");
    EXPECT_EQ(message.getField(Tags::SenderCompID), "SENDER");
    EXPECT_EQ(message.getField(Tags::TargetCompID), "TARGET");
    EXPECT_EQ(message.getField(Tags::MsgSeqNum), "1");
    EXPECT_EQ(message.getField(Tags::SendingTime), "20230101-12:30:00");
    EXPECT_EQ(message.getField(Tags::ClOrdID), "ORDER123");
    EXPECT_EQ(message.getField(Tags::HandlInst), "1");
    EXPECT_EQ(message.getField(Tags::Symbol), "SYMBOL");
    EXPECT_EQ(message.getField(Tags::Side), "1");
    EXPECT_EQ(message.getField(Tags::TransactTime), "20230101-12:30:00");
    EXPECT_EQ(message.getField(Tags::OrderQty), "100");
    EXPECT_EQ(message.getField(Tags::OrdType), "2");
    EXPECT_EQ(message.getField(Tags::Price), "50.00");
    EXPECT_EQ(message.getField(Tags::CheckSum), "123");
}

TEST(MessageParserTest, ParseInvalidMessage) {
    // Arrange
    std::string messageString = "Invalid message";

    // Act
    Message message = MessageParser::parse(messageString);

    // Assert - should return empty message for invalid input
    EXPECT_EQ(message.getFields().size(), 0);
}

TEST(MessageParserTest, ComposeValidMessage) {
    // Arrange
    Message message;
    message.setField(Tags::BeginString, "FIX.4.2");
    message.setField(Tags::BodyLength, "");
    message.setField(Tags::MsgType, "D");
    message.setField(Tags::SenderCompID, "SENDER");
    message.setField(Tags::TargetCompID, "TARGET");
    message.setField(Tags::MsgSeqNum, "1");
    message.setField(Tags::SendingTime, "20230101-12:30:00");
    message.setField(Tags::ClOrdID, "ORDER123");
    message.setField(Tags::HandlInst, "1");
    message.setField(Tags::Symbol, "SYMBOL");
    message.setField(Tags::Side, "1");
    message.setField(Tags::TransactTime, "20230101-12:30:00");
    message.setField(Tags::OrderQty, "100");
    message.setField(Tags::OrdType, "2");
    message.setField(Tags::Price, "50.00");

    // Act
    std::string composedMessage = MessageParser::compose(message);

    // Assert - check that message contains expected fields in proper FIX format
    EXPECT_TRUE(composedMessage.find("8=FIX.4.2\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("35=D\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("49=SENDER\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("56=TARGET\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("55=SYMBOL\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("54=1\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("38=100\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("44=50.00\001") != std::string::npos);
    EXPECT_TRUE(composedMessage.find("10=") != std::string::npos); // Checksum field
}