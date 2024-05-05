#include "fix/MessageParser.h"
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

    // Act & Assert
    EXPECT_THROW(MessageParser::parse(messageString), std::runtime_error);
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

    // Assert
    std::string expectedMessage = "8=FIX.4.2|9=145|35=D|49=SENDER|56=TARGET|34=1|52=20230101-12:30:00|11=ORDER123|21=1|55=SYMBOL|54=1|60=20230101-12:30:00|38=100|40=2|44=50.00|10=123|";
    EXPECT_EQ(composedMessage, expectedMessage);
}