#include "service/TradingService.h"
#include "service/FixService.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;

class MockFixService : public service::FixService {
public:
    MockFixService() : FixService(fix::Config(""), mockApplication_) {}

    MOCK_METHOD(void, sendOrder, (const model::Order&), (override));
    MOCK_METHOD(void, cancelOrder, (const std::string&), (override));

private:
    fix::Application mockApplication_;
};

class TradingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        fixService_ = std::make_unique<MockFixService>();
        tradingService_ = std::make_unique<service::TradingService>(*fixService_);
    }

    std::unique_ptr<MockFixService> fixService_;
    std::unique_ptr<service::TradingService> tradingService_;
};

TEST_F(TradingServiceTest, PlaceOrder) {
    // Arrange
    model::Order order("order123", "SYMBOL", 100.0, 10, model::Order::Side::Buy, model::Order::OrderType::Limit);

    // Expect the sendOrder method of FixService to be called with the order
    EXPECT_CALL(*fixService_, sendOrder(testing::Eq(order)));

    // Act
    tradingService_->placeOrder(order);
}

TEST_F(TradingServiceTest, CancelOrder) {
    // Arrange
    std::string orderId = "order456";

    // Expect the cancelOrder method of FixService to be called with the order ID
    EXPECT_CALL(*fixService_, cancelOrder(orderId));

    // Act
    tradingService_->cancelOrder(orderId);
}

TEST_F(TradingServiceTest, ProcessTrade) {
    // Arrange
    model::Trade trade("trade123", "order789", "SYMBOL", 100.0, 5, model::Trade::Side::Buy, std::chrono::system_clock::now());

    // Act
    tradingService_->onOrderExecuted("order789", 100.0, 5);

    // Assert
    // Check if the trade is processed correctly
    // For example, you can check if the trade is added to the trading service's internal trade list
    EXPECT_EQ(tradingService_->getTrades().size(), 1);
    EXPECT_EQ(tradingService_->getTrades()[0].getId(), "trade123");
    EXPECT_EQ(tradingService_->getTrades()[0].getOrderId(), "order789");
    EXPECT_EQ(tradingService_->getTrades()[0].getSymbol(), "SYMBOL");
    EXPECT_EQ(tradingService_->getTrades()[0].getPrice(), 100.0);
    EXPECT_EQ(tradingService_->getTrades()[0].getQuantity(), 5);
    EXPECT_EQ(tradingService_->getTrades()[0].getSide(), model::Trade::Side::Buy);
}

// Add more test cases for other TradingService methods