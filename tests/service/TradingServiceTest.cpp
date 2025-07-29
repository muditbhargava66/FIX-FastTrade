#include "service/TradingService.h"
#include "service/FixService.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;

class MockApplication : public fix::Application {
public:
    MOCK_METHOD(void, onCreate, (const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, onLogon, (const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, onLogout, (const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, toAdmin, (fix::Message& message, const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, fromAdmin, (const fix::Message& message, const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, toApp, (fix::Message& message, const fix::SessionID& sessionID), (override));
    MOCK_METHOD(void, fromApp, (const fix::Message& message, const fix::SessionID& sessionID), (override));
};

class MockFixService : public service::FixService {
public:
    MockFixService() : FixService(std::make_shared<MockApplication>()) {}

    MOCK_METHOD(bool, sendNewOrder, (const model::Order&));
    MOCK_METHOD(bool, cancelOrder, (const std::string&));
};

class TradingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        fixService_ = std::make_unique<MockFixService>();
        tradingService_ = std::make_unique<service::TradingService>();
    }

    std::unique_ptr<MockFixService> fixService_;
    std::unique_ptr<service::TradingService> tradingService_;
};

TEST_F(TradingServiceTest, ProcessOrder) {
    // Arrange
    model::Order order("order123", "SYMBOL", 100.0, 10, model::Order::Side::Buy, model::Order::OrderType::Limit);

    // Act
    tradingService_->start();
    bool result = tradingService_->processOrder(order);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(tradingService_->getTrades().size(), 1u);
}

TEST_F(TradingServiceTest, ProcessOrderWhenStopped) {
    // Arrange
    model::Order order("order456", "SYMBOL", 100.0, 10, model::Order::Side::Buy, model::Order::OrderType::Limit);

    // Act (service not started)
    bool result = tradingService_->processOrder(order);

    // Assert
    EXPECT_FALSE(result);
    EXPECT_EQ(tradingService_->getTrades().size(), 0u);
}

TEST_F(TradingServiceTest, GetTrades) {
    // Arrange
    model::Order order1("order1", "SYMBOL1", 100.0, 10, model::Order::Side::Buy, model::Order::OrderType::Limit);
    model::Order order2("order2", "SYMBOL2", 200.0, 20, model::Order::Side::Sell, model::Order::OrderType::Market);

    // Act
    tradingService_->start();
    tradingService_->processOrder(order1);
    tradingService_->processOrder(order2);

    // Assert
    auto trades = tradingService_->getTrades();
    EXPECT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].getOrderId(), "order1");
    EXPECT_EQ(trades[1].getOrderId(), "order2");
}

// Add more test cases for other TradingService methods