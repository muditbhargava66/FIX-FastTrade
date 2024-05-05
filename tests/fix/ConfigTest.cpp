#include "fix/Config.h"
#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

TEST(ConfigTest, LoadFromFile) {
    // Arrange
    std::string configFilePath = "test_config.txt";
    std::ofstream configFile(configFilePath);
    configFile << "ConnectionType=acceptor\n";
    configFile << "SocketAcceptPort=5001\n";
    configFile << "SenderCompID=SENDER\n";
    configFile << "TargetCompID=TARGET\n";
    configFile.close();

    // Act
    fix::Config config(configFilePath);

    // Assert
    EXPECT_EQ(config.getString("ConnectionType"), "acceptor");
    EXPECT_EQ(config.getInt("SocketAcceptPort"), 5001);
    EXPECT_EQ(config.getString("SenderCompID"), "SENDER");
    EXPECT_EQ(config.getString("TargetCompID"), "TARGET");

    // Clean up
    std::remove(configFilePath.c_str());
}

TEST(ConfigTest, GetString) {
    // Arrange
    std::string configFilePath = "test_config.txt";
    std::ofstream configFile(configFilePath);
    configFile << "ConnectionType=acceptor\n";
    configFile << "SenderCompID=SENDER\n";
    configFile.close();

    fix::Config config(configFilePath);

    // Act
    std::string connectionType = config.getString("ConnectionType");
    std::string senderCompID = config.getString("SenderCompID");
    std::string defaultValue = config.getString("NonExistentKey", "DEFAULT");

    // Assert
    EXPECT_EQ(connectionType, "acceptor");
    EXPECT_EQ(senderCompID, "SENDER");
    EXPECT_EQ(defaultValue, "DEFAULT");

    // Clean up
    std::remove(configFilePath.c_str());
}

TEST(ConfigTest, GetInt) {
    // Arrange
    std::string configFilePath = "test_config.txt";
    std::ofstream configFile(configFilePath);
    configFile << "SocketAcceptPort=5001\n";
    configFile.close();

    fix::Config config(configFilePath);

    // Act
    int socketAcceptPort = config.getInt("SocketAcceptPort");
    int defaultValue = config.getInt("NonExistentKey", 1234);

    // Assert
    EXPECT_EQ(socketAcceptPort, 5001);
    EXPECT_EQ(defaultValue, 1234);

    // Clean up
    std::remove(configFilePath.c_str());
}

TEST(ConfigTest, GetBool) {
    // Arrange
    std::string configFilePath = "test_config.txt";
    std::ofstream configFile(configFilePath);
    configFile << "EnableLogging=true\n";
    configFile << "DisableHeartbeats=false\n";
    configFile.close();

    fix::Config config(configFilePath);

    // Act
    bool enableLogging = config.getBool("EnableLogging");
    bool disableHeartbeats = config.getBool("DisableHeartbeats");
    bool defaultValue = config.getBool("NonExistentKey", true);

    // Assert
    EXPECT_TRUE(enableLogging);
    EXPECT_FALSE(disableHeartbeats);
    EXPECT_TRUE(defaultValue);

    // Clean up
    std::remove(configFilePath.c_str());
}