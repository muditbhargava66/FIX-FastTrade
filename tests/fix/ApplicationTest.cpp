#include "fix/Application.h"
#include "fix/Message.h"
#include "fix/Session.h"
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

TEST(ApplicationTest, OnCreate) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");

    // Expect the onCreate method to be called with the correct session ID
    EXPECT_CALL(application, onCreate(sessionID)).Times(1);

    // Act
    application.onCreate(sessionID);
}

TEST(ApplicationTest, OnLogon) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");

    // Expect the onLogon method to be called with the correct session ID
    EXPECT_CALL(application, onLogon(sessionID)).Times(1);

    // Act
    application.onLogon(sessionID);
}

TEST(ApplicationTest, OnLogout) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");

    // Expect the onLogout method to be called with the correct session ID
    EXPECT_CALL(application, onLogout(sessionID)).Times(1);

    // Act
    application.onLogout(sessionID);
}

TEST(ApplicationTest, ToAdmin) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");
    fix::Message message;

    // Expect the toAdmin method to be called with the correct message and session ID
    EXPECT_CALL(application, toAdmin(_, sessionID)).Times(1);

    // Act
    application.toAdmin(message, sessionID);
}

TEST(ApplicationTest, FromAdmin) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");
    fix::Message message;

    // Expect the fromAdmin method to be called with the correct message and session ID
    EXPECT_CALL(application, fromAdmin(_, sessionID)).Times(1);

    // Act
    application.fromAdmin(message, sessionID);
}

TEST(ApplicationTest, ToApp) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");
    fix::Message message;

    // Expect the toApp method to be called with the correct message and session ID
    EXPECT_CALL(application, toApp(_, sessionID)).Times(1);

    // Act
    application.toApp(message, sessionID);
}

TEST(ApplicationTest, FromApp) {
    // Arrange
    MockApplication application;
    fix::SessionID sessionID("FIX.4.2", "SENDER", "TARGET");
    fix::Message message;

    // Expect the fromApp method to be called with the correct message and session ID
    EXPECT_CALL(application, fromApp(_, sessionID)).Times(1);

    // Act
    application.fromApp(message, sessionID);
}