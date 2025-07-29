# Multi-stage Dockerfile for FIX-FastTrade

# Build stage
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libboost-all-dev \
    libquickfix-dev \
    libtbb-dev \
    libbenchmark-dev \
    libspdlog-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Initialize submodules if present
RUN if [ -f ".gitmodules" ]; then \
        git submodule update --init --recursive; \
    fi

# Build the application
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04 AS runtime

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    libboost-filesystem1.74.0 \
    libboost-date-time1.74.0 \
    libboost-program-options1.74.0 \
    libquickfix17 \
    libtbb12 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1000 fixtrader

# Copy built application
COPY --from=builder /app/build/bin/fix-fasttrade /usr/local/bin/
COPY --from=builder /app/config/ /app/config/

# Set ownership
RUN chown -R fixtrader:fixtrader /app

# Switch to non-root user
USER fixtrader

# Set working directory
WORKDIR /app

# Expose default port (configurable)
EXPOSE 9878

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD pgrep fix-fasttrade || exit 1

# Default command
CMD ["fix-fasttrade", "--config", "config/fix-config.xml"]