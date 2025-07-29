Performance Tuning Guide
========================

This guide covers advanced performance tuning techniques to achieve sub-microsecond latency and maximum throughput with FIX-FastTrade.

Performance Overview
--------------------

FIX-FastTrade is designed for ultra-low latency trading with the following performance characteristics:

* **Latency**: Sub-microsecond message processing (< 1μs)
* **Throughput**: 1M+ messages/second sustained
* **Memory**: Zero-copy operations with custom allocators
* **CPU**: Multi-core scaling with 90%+ efficiency

Platform-Specific Performance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Performance by Platform
   :header-rows: 1
   :widths: 20 15 20 20 25

   * - Platform
     - Latency (μs)
     - Throughput (msg/s)
     - Memory Usage
     - CPU Efficiency
   * - macOS ARM64
     - 0.8
     - 2.5M
     - Excellent
     - Native NEON
   * - macOS x86_64
     - 0.6
     - 4.0M
     - Excellent
     - AVX2 optimized
   * - Linux x86_64
     - 0.5
     - 5.0M
     - Excellent
     - Full SIMD
   * - Linux ARM64
     - 0.9
     - 2.2M
     - Excellent
     - NEON optimized

System-Level Optimizations
--------------------------

CPU Configuration
~~~~~~~~~~~~~~~~~

**CPU Affinity and Isolation**

Bind critical threads to specific CPU cores:

.. code-block:: bash

   # Isolate CPUs for FIX-FastTrade (add to kernel boot parameters)
   isolcpus=1,2,3,4
   
   # Set CPU governor to performance mode
   echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
   
   # Disable CPU frequency scaling
   echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

**NUMA Optimization**

For multi-socket systems:

.. code-block:: bash

   # Check NUMA topology
   numactl --hardware
   
   # Run FIX-FastTrade on specific NUMA node
   numactl --cpunodebind=0 --membind=0 ./bin/fix-fasttrade --config config/fix-config.xml

Memory Optimization
~~~~~~~~~~~~~~~~~~~

**Huge Pages Configuration**

Enable huge pages for better memory performance:

.. code-block:: bash

   # Configure huge pages (add to /etc/sysctl.conf)
   vm.nr_hugepages = 1024
   
   # Apply settings
   sudo sysctl -p
   
   # Verify huge pages
   cat /proc/meminfo | grep Huge

**Memory Locking**

Lock memory pages to prevent swapping:

.. code-block:: bash

   # Increase memory lock limits (add to /etc/security/limits.conf)
   * soft memlock unlimited
   * hard memlock unlimited
   
   # Or run with sudo for memory locking
   sudo ./bin/fix-fasttrade --config config/fix-config.xml --memory-lock

Network Optimization
~~~~~~~~~~~~~~~~~~~~

**Network Interface Tuning**

Optimize network interface for low latency:

.. code-block:: bash

   # Increase network buffer sizes
   echo 'net.core.rmem_max = 134217728' >> /etc/sysctl.conf
   echo 'net.core.wmem_max = 134217728' >> /etc/sysctl.conf
   echo 'net.ipv4.tcp_rmem = 4096 87380 134217728' >> /etc/sysctl.conf
   echo 'net.ipv4.tcp_wmem = 4096 65536 134217728' >> /etc/sysctl.conf
   
   # Disable TCP timestamp and SACK
   echo 'net.ipv4.tcp_timestamps = 0' >> /etc/sysctl.conf
   echo 'net.ipv4.tcp_sack = 0' >> /etc/sysctl.conf
   
   # Apply settings
   sudo sysctl -p

**Network Interface Card (NIC) Tuning**

.. code-block:: bash

   # Set interrupt affinity
   echo 2 > /proc/irq/24/smp_affinity  # Bind NIC interrupts to CPU 1
   
   # Increase ring buffer sizes
   ethtool -G eth0 rx 4096 tx 4096
   
   # Enable hardware timestamping (if supported)
   ethtool -T eth0

Application-Level Optimizations
-------------------------------

Configuration Tuning
~~~~~~~~~~~~~~~~~~~~

**High-Performance Configuration**

.. code-block:: xml

   <performance>
       <!-- CPU affinity for critical threads -->
       <cpuAffinity>true</cpuAffinity>
       <cpuMain>0</cpuMain>
       <cpuFix>1</cpuFix>
       <cpuOrder>2</cpuOrder>
       <cpuMarket>3</cpuMarket>
       
       <!-- Memory optimization -->
       <memoryLock>true</memoryLock>
       <memoryPoolSize>2097152</memoryPoolSize>
       <preallocateMemory>true</preallocateMemory>
       
       <!-- Thread priority -->
       <threadPriority>-15</threadPriority>
       <useRealTimeScheduling>true</useRealTimeScheduling>
       
       <!-- Message processing -->
       <messageBufferSize>8192</messageBufferSize>
       <maxConcurrentMessages>50000</maxConcurrentMessages>
       <zeroCopyEnabled>true</zeroCopyEnabled>
   </performance>

**Network Configuration for Low Latency**

.. code-block:: xml

   <session>
       <!-- TCP optimization -->
       <tcpNoDelay>true</tcpNoDelay>
       <socketBufferSize>131072</socketBufferSize>
       
       <!-- Reduce timeouts -->
       <connectTimeout>5</connectTimeout>
       <heartBtInt>15</heartBtInt>
       
       <!-- Disable unnecessary features -->
       <validateMessages>false</validateMessages>
       <logIncomingMessages>false</logIncomingMessages>
       <logOutgoingMessages>false</logOutgoingMessages>
   </session>

Compiler Optimizations
~~~~~~~~~~~~~~~~~~~~~~

**Build with Maximum Optimization**

.. code-block:: bash

   # Build with aggressive optimizations
   mkdir build && cd build
   cmake .. \
     -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto -funroll-loops" \
     -DENABLE_SIMD=ON \
     -DENABLE_PROFILE_GUIDED_OPTIMIZATION=ON
   
   make -j$(nproc)

**Profile-Guided Optimization (PGO)**

.. code-block:: bash

   # Build with PGO instrumentation
   cmake .. -DCMAKE_CXX_FLAGS="-fprofile-generate"
   make -j$(nproc)
   
   # Run training workload
   ./bin/fix-fasttrade --config config/training-config.xml
   
   # Rebuild with PGO optimization
   cmake .. -DCMAKE_CXX_FLAGS="-fprofile-use"
   make -j$(nproc)

Runtime Optimizations
~~~~~~~~~~~~~~~~~~~~~

**Command Line Tuning**

.. code-block:: bash

   # Maximum performance configuration
   sudo ./bin/fix-fasttrade \
     --config config/high-performance.xml \
     --cpu-main 0 \
     --cpu-fix 1 \
     --cpu-order 2 \
     --cpu-market 3 \
     --memory-lock \
     --priority -15 \
     --stats-interval 300

**Environment Variables**

.. code-block:: bash

   # Disable address space randomization
   export ADDR_NO_RANDOMIZE=1
   
   # Set CPU affinity mask
   export CPU_AFFINITY_MASK=0x0F
   
   # Optimize memory allocation
   export MALLOC_ARENA_MAX=1
   export MALLOC_MMAP_THRESHOLD_=131072

Monitoring and Measurement
--------------------------

Performance Metrics
~~~~~~~~~~~~~~~~~~~

**Built-in Statistics**

FIX-FastTrade provides real-time performance metrics:

.. code-block:: text

   === Performance Statistics ===
   Uptime: 3600 seconds
   Messages processed: 18,000,000
   Orders processed: 1,200,000
   Message rate: 5,000 msg/sec
   Order rate: 333 orders/sec
   
   Latency Statistics:
   - Mean: 0.45μs
   - P50: 0.42μs
   - P95: 0.68μs
   - P99: 1.23μs
   - P99.9: 2.45μs
   
   Queue Statistics:
   - Order queue size: 0
   - Message queue size: 0
   - Memory pool utilization: 23%

**External Monitoring Tools**

.. code-block:: bash

   # CPU usage monitoring
   top -p $(pgrep fix-fasttrade)
   
   # Memory usage
   pmap -x $(pgrep fix-fasttrade)
   
   # Network statistics
   ss -i dst :9878
   
   # System-wide latency
   cyclictest -p 80 -t5 -m -n

Latency Measurement
~~~~~~~~~~~~~~~~~~~

**Timestamping**

Enable hardware timestamping for accurate measurements:

.. code-block:: bash

   # Check hardware timestamping support
   ethtool -T eth0
   
   # Enable hardware timestamping in application
   export ENABLE_HW_TIMESTAMPING=1

**Latency Testing**

.. code-block:: bash

   # Run latency benchmark
   ./bin/fix-fasttrade-benchmark \
     --config config/benchmark.xml \
     --test-duration 300 \
     --message-rate 10000

Troubleshooting Performance Issues
----------------------------------

Common Performance Problems
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**High Latency Symptoms**

.. code-block:: text

   P99 latency > 10μs
   Message processing rate < 100,000 msg/sec
   High CPU wait time

**Diagnostic Steps**

.. code-block:: bash

   # Check CPU frequency scaling
   cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq
   
   # Monitor context switches
   vmstat 1
   
   # Check for memory swapping
   free -h && cat /proc/swaps
   
   # Network interface statistics
   cat /proc/net/dev

**Memory Issues**

.. code-block:: bash

   # Check for memory leaks
   valgrind --tool=memcheck ./bin/fix-fasttrade --config config/test.xml
   
   # Monitor memory allocation
   strace -e trace=mmap,munmap,brk ./bin/fix-fasttrade --config config/test.xml

**Network Issues**

.. code-block:: bash

   # Check network latency
   ping -c 10 fix.exchange.com
   
   # Monitor network drops
   netstat -i
   
   # Check TCP retransmissions
   ss -i dst :9878

Performance Tuning Checklist
----------------------------

System Level
~~~~~~~~~~~~

* [ ] CPU governor set to performance mode
* [ ] CPU frequency scaling disabled
* [ ] Huge pages configured and enabled
* [ ] Memory locking limits increased
* [ ] Network buffers optimized
* [ ] Interrupt affinity configured
* [ ] NUMA topology optimized

Application Level
~~~~~~~~~~~~~~~~~

* [ ] CPU affinity configured for all threads
* [ ] Memory locking enabled
* [ ] Real-time thread priorities set
* [ ] Zero-copy message processing enabled
* [ ] Message validation disabled (if safe)
* [ ] Logging minimized for production
* [ ] Connection pooling optimized

Build Configuration
~~~~~~~~~~~~~~~~~~~

* [ ] Release build with maximum optimization
* [ ] Link-time optimization (LTO) enabled
* [ ] Profile-guided optimization applied
* [ ] SIMD instructions enabled
* [ ] Native CPU architecture targeting

Monitoring Setup
~~~~~~~~~~~~~~~~

* [ ] Real-time performance metrics enabled
* [ ] Latency percentiles tracked
* [ ] System resource monitoring active
* [ ] Network performance monitoring
* [ ] Alert thresholds configured

Expected Performance Targets
----------------------------

After applying these optimizations, you should achieve:

* **Latency**: P99 < 2μs, P99.9 < 5μs
* **Throughput**: > 1M messages/second
* **CPU Usage**: < 50% on dedicated cores
* **Memory**: < 1GB resident set size
* **Network**: < 0.1ms round-trip time to exchange

For specific performance requirements or advanced tuning, consult the :doc:``../reference/troubleshooting/index`` section or contact support.