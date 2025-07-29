# FIX-FastTrade Configuration Files

This directory contains configuration files for the FIX-FastTrade system.

## Configuration Files

### Main Configuration
- `fix-config.xml` - Main production configuration file
- `fix-config.cfg` - Legacy key-value format configuration (for backward compatibility)

### Example Configurations
- `examples/basic-config.xml` - Minimal working configuration for getting started
- `examples/advanced-config.xml` - Full-featured configuration demonstrating all options
- `examples/test-config.xml` - Configuration optimized for testing and development

### Schema
- `schemas/fix-config.xsd` - XML Schema Definition for validating configuration files

## Configuration Structure

The XML configuration files are structured as follows:

### Connection Section
- **Type**: `initiator` or `acceptor`
- **Host**: Target host address
- **Port**: Target port number
- **HeartBtInt**: Heartbeat interval in seconds
- **ReconnectInterval**: Reconnection interval in seconds
- **Socket settings**: Buffer sizes and TCP options

### Session Section
- **BeginString**: FIX protocol version (e.g., `FIX.4.4`)
- **SenderCompID**: Sender company identifier
- **TargetCompID**: Target company identifier
- **SessionQualifier**: Optional session qualifier
- **Time settings**: Start/end times and timezone options
- **Persistence settings**: Message storage and reset options

### Performance Section
- **Latency options**: Timing and performance measurement settings
- **Validation options**: Message validation controls
- **Zero-copy options**: Memory optimization settings

### Logging Section
- **File paths**: Log and store directory paths
- **Log level**: DEBUG, INFO, WARN, or ERROR
- **Output options**: Console and file logging controls

## SessionID Compatibility

The configuration files are designed to work with the current SessionID implementation:

```cpp
SessionID sessionId(beginString, senderCompID, targetCompID);
```

The SessionID constructor expects:
- `beginString` from the Session/BeginString element
- `senderCompID` from the Session/SenderCompID element  
- `targetCompID` from the Session/TargetCompID element

## Validation

All XML configuration files can be validated against the schema:

```bash
xmllint --schema config/schemas/fix-config.xsd --noout config/fix-config.xml
```

## Usage Examples

### Basic Usage
```bash
# Use the basic configuration for development
./fix-fasttrade --config config/examples/basic-config.xml
```

### Production Usage
```bash
# Use the main configuration for production
./fix-fasttrade --config config/fix-config.xml
```

### Testing
```bash
# Use the test configuration for automated testing
./fix-fasttrade --config config/examples/test-config.xml
```

## Configuration Tips

1. **For Development**: Use `basic-config.xml` with minimal settings
2. **For Production**: Use `fix-config.xml` or `advanced-config.xml` with full optimization
3. **For Testing**: Use `test-config.xml` with validation enabled and verbose logging
4. **Performance**: Disable validation options in production for better performance
5. **Debugging**: Enable DEBUG logging and console output for troubleshooting