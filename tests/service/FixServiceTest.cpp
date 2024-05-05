#include "service/FixService.h"
#include "fix/Application.h"
#include "fix/Message.h"
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

class FixServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_unique<fix::Config>("path/to/config.txt");
        fixService_ = std::make_unique<service::FixService>(*config_, mockApplication_);
    }

    std::unique_ptr<fix::Config> config_;
    std::unique_ptr<service::FixService> fixService_;
    MockApplication mockApplication_;
};

TEST_F(FixServiceTest, SendOrder) {
    // Arrange
    model::Order order("order123", "SYMBOL", 100.0, 10, model::Order::Side::Buy, model::Order::OrderType::Limit);

    // Expect the toApp method to be called with the appropriate message
    EXPECT_CALL(mockApplication_, toApp(_, _))
        .WillOnce([](fix::Message& message, const fix::SessionID&) {
            EXPECT_EQ(message.getField(fix::Tags::Symbol), "SYMBOL");
            EXPECT_EQ(message.getField(fix::Tags::Side), "1");
            EXPECT_EQ(message.getField(fix::Tags::OrderQty), "10");
            EXPECT_EQ(message.getField(fix::Tags::Price), "100.000000");
            EXPECT_EQ(message.getField(fix::Tags::OrdType), "2");
        });

    // Act
    fixService_->sendOrder(order);
}

TEST_F(FixServiceTest, CancelOrder) {
    // Arrange
    std::string orderId = "order456";

    // Expect the toApp method to be called with the appropriate message
    EXPECT_CALL(mockApplication_, toApp(_, _))
        .WillOnce([orderId](fix::Message& message, const fix::SessionID&) {
            EXPECT_EQ(message.getField(fix::Tags::OrigClOrdID), orderId);
            EXPECT_EQ(message.getField(fix::Tags::ClOrdID), "Cancel_" + orderId);
            EXPECT_EQ(message.getField(fix::Tags::Side), "1");
        });

    // Act
    fixService_->cancelOrder(orderId);
}

// Add more test cases for other FixService methods