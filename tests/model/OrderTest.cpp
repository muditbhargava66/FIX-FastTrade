#include "model/Order.h"
#include "gtest/gtest.h"

using namespace model;

TEST(OrderTest, Constructor) {
    // Arrange
    std::string id = "order123";
    std::string symbol = "SYMBOL";
    double price = 100.0;
    int quantity = 10;
    Order::Side side = Order::Side::Buy;
    Order::OrderType type = Order::OrderType::Limit;

    // Act
    Order order(id, symbol, price, quantity, side, type);

    // Assert
    EXPECT_EQ(order.getId(), id);
    EXPECT_EQ(order.getSymbol(), symbol);
    EXPECT_EQ(order.getPrice(), price);
    EXPECT_EQ(order.getQuantity(), quantity);
    EXPECT_EQ(order.getSide(), side);
    EXPECT_EQ(order.getType(), type);
    EXPECT_EQ(order.getStatus(), Order::OrderStatus::New);
}

TEST(OrderTest, SetQuantity) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act
    order.setQuantity(5);

    // Assert
    EXPECT_EQ(order.getQuantity(), 5);
}

TEST(OrderTest, SetQuantityThrowsOnNegativeValue) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act & Assert
    EXPECT_THROW(order.setQuantity(-5), std::invalid_argument);
}

TEST(OrderTest, SetStatus) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act
    order.setStatus(Order::OrderStatus::PartiallyFilled);

    // Assert
    EXPECT_EQ(order.getStatus(), Order::OrderStatus::PartiallyFilled);
}

TEST(OrderTest, IsFilled) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act
    order.setStatus(Order::OrderStatus::Filled);

    // Assert
    EXPECT_TRUE(order.isFilled());
}

TEST(OrderTest, IsPartiallyFilled) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act
    order.setStatus(Order::OrderStatus::PartiallyFilled);

    // Assert
    EXPECT_TRUE(order.isPartiallyFilled());
}

TEST(OrderTest, IsCancelled) {
    // Arrange
    Order order("order123", "SYMBOL", 100.0, 10, Order::Side::Buy, Order::OrderType::Limit);

    // Act
    order.setStatus(Order::OrderStatus::Cancelled);

    // Assert
    EXPECT_TRUE(order.isCancelled());
}