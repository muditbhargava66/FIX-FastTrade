#include "model/Trade.h"
#include "gtest/gtest.h"
#include <chrono>

using namespace model;

TEST(TradeTest, Constructor) {
    // Arrange
    std::string id = "trade123";
    std::string orderId = "order456";
    std::string symbol = "SYMBOL";
    double price = 100.0;
    int quantity = 5;
    Trade::Side side = Trade::Side::Sell;
    auto timestamp = std::chrono::system_clock::now();

    // Act
    Trade trade(id, orderId, symbol, price, quantity, side, timestamp);

    // Assert
    EXPECT_EQ(trade.getId(), id);
    EXPECT_EQ(trade.getOrderId(), orderId);
    EXPECT_EQ(trade.getSymbol(), symbol);
    EXPECT_EQ(trade.getPrice(), price);
    EXPECT_EQ(trade.getQuantity(), quantity);
    EXPECT_EQ(trade.getSide(), side);
    EXPECT_EQ(trade.getTimestamp(), timestamp);
}