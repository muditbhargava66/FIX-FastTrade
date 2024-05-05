#include "fix/Session.h"
#include "fix/Config.h"
#include "fix/MessageParser.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::Return;

class MockApplication : public fix::Application {
public:
    MOCK_METHOD(void, onCreate, (const fix::SessionID&), (override));
    MOCK_METHOD(void, onLogon, (const fix::SessionID&), (override));
    MOCK_METHOD(void, onLogout, (const fix::SessionID&), (override));
    MOCK_METHOD(void, toAdmin, (fix::Message&, const fix::SessionID&), (override));
    MOCK_METHOD(void, fromAdmin, (const fix::Message&, const fix::SessionID&), (override));
    MOCK_METHOD(void, toApp, (fix::Message&, const fix::SessionID&), (override));
    MOCK_METHOD(void, fromApp, (const fix::Message&, const fix::SessionID&), (override));
};

class SessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_unique<fix::Config>("test_config.txt");
        session_ = std::make_unique<fix::Session>(*config_, mockApplication_);
    }

    std::unique_ptr<fix::Config> config_;
    std::unique_ptr<fix::Session> session_;
    MockApplication mockApplication_;
};

TEST_F(SessionTest, Connect) {
    // Arrange
    EXPECT_CALL(mockApplication_, onCreate(_)).Times(1);
    EXPECT_CALL(mockApplication_, onLogon(_)).Times(1);

    // Act
    session_->connect();

    // Assert
    EXPECT_TRUE(session_->isConnected());
}

TEST_F(SessionTest, Disconnect) {
    // Arrange
    EXPECT_CALL(mockApplication_, onCreate(_)).Times(1);
    EXPECT_CALL(mockApplication_, onLogon(_)).Times(1);
    EXPECT_CALL(mockApplication_, onLogout(_)).Times(1);

    session_->connect();

    // Act
    session_->disconnect();

    // Assert
    EXPECT_FALSE(session_->isConnected());
}

TEST_F(SessionTest, SendMessage) {
    // Arrange
    EXPECT_CALL(mockApplication_, onCreate(_)).Times(1);
    EXPECT_CALL(mockApplication_, onLogon(_)).Times(1);
    EXPECT_CALL(mockApplication_, toApp(_, _)).Times(1);

    fix::Message message;
    message.setField(fix::Tags::MsgType, "D");
    message.setField(fix::Tags::Symbol, "SYMBOL");

    session_->connect();

    // Act
    session_->send(message);
}

TEST_F(SessionTest, ReceiveMessage) {
    // Arrange
    EXPECT_CALL(mockApplication_, onCreate(_)).Times(1);
    EXPECT_CALL(mockApplication_, onLogon(_)).Times(1);
    EXPECT_CALL(mockApplication_, fromApp(_, _)).Times(1);

    std::string messageString = "8=FIX.4.2|9=145|35=D|49=SENDER|56=TARGET|34=1|52=20230101-12:30:00|11=ORDER123|21=1|55=SYMBOL|54=1|60=20230101-12:30:00|38=100|40=2|44=50.00|10=123|";

    session_->connect();

    // Act
    fix::Message receivedMessage;
    bool result = session_->receive(receivedMessage);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(receivedMessage.getField(fix::Tags::MsgType), "D");
    EXPECT_EQ(receivedMessage.getField(fix::Tags::Symbol), "SYMBOL");
}