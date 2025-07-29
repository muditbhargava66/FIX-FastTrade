Troubleshooting Guide
=====================

This section provides comprehensive troubleshooting information for FIX-FastTrade across different platforms and scenarios.

Quick Troubleshooting
---------------------

Common Issues and Quick Fixes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Application Won't Start**

.. code-block:: bash

   # Check configuration file
   xmllint --noout config/fix-config.xml
   
   # Verify dependencies
   ldd bin/fix-fasttrade  # Linux
   otool -L bin/fix-fasttrade  # macOS

**Connection Issues**

.. code-block:: bash

   # Test network connectivity
   telnet fix.exchange.com 9878
   
   # Check firewall settings
   sudo iptables -L | grep 9878  # Linux
   sudo pfctl -sr | grep 9878    # macOS

**Performance Issues**

.. code-block:: bash

   # Check CPU usage
   top -p $(pgrep fix-fasttrade)
   
   # Monitor memory usage
   ps -o pid,ppid,cmd,%mem,%cpu --sort=-%mem -C fix-fasttrade

Platform-Specific Troubleshooting
---------------------------------

.. toctree::
   :maxdepth: 2

   linux-troubleshooting
   macos-troubleshooting
   windows-troubleshooting

Common Error Messages
---------------------

Build Errors
~~~~~~~~~~~~

**"CMake version too old"**

.. code-block:: text

   CMake 3.14 or higher is required. You are running version 3.10.2

*Solution*: Upgrade CMake to version 3.14 or higher

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt remove cmake
   sudo snap install cmake --classic
   
   # macOS
   brew upgrade cmake
   
   # From source
   wget https://cmake.org/files/v3.25/cmake-3.25.0.tar.gz
   tar -xzf cmake-3.25.0.tar.gz
   cd cmake-3.25.0
   ./bootstrap && make && sudo make install

**"Boost library not found"**

.. code-block:: text

   Could NOT find Boost (missing: Boost_INCLUDE_DIR)

*Solution*: Install Boost development libraries

.. code-block:: bash

   # Ubuntu/Debian
   sudo apt install libboost-all-dev
   
   # CentOS/RHEL
   sudo yum install boost-devel
   
   # macOS
   brew install boost

**"QuickFIX library not found"**

.. code-block:: text

   Could NOT find QuickFIX

*Solution*: Install QuickFIX or use submodule

.. code-block:: bash

   # Use included submodule
   git submodule update --init --recursive
   
   # Or install system package
   sudo apt install libquickfix-dev  # Ubuntu/Debian

Runtime Errors
~~~~~~~~~~~~~~

**"Permission denied" on startup**

.. code-block:: text

   bind: Permission denied

*Solution*: Use unprivileged port or run with appropriate permissions

.. code-block:: bash

   # Change port in configuration (use port > 1024)
   sed -i 's/<port>443<\/port>/<port>9878<\/port>/' config/fix-config.xml
   
   # Or run with sudo for privileged ports
   sudo ./bin/fix-fasttrade --config config/fix-config.xml

**"Address already in use"**

.. code-block:: text

   Cannot bind to port 9878: Address already in use

*Solution*: Find and stop conflicting process or use different port

.. code-block:: bash

   # Find process using the port
   lsof -i :9878
   netstat -tlnp | grep :9878
   
   # Kill the process
   kill $(lsof -t -i:9878)
   
   # Or use different port
   <port>9879</port>

**"Shared library not found"**

.. code-block:: text

   error while loading shared libraries: libboost_system.so.1.71.0: cannot open shared object file

*Solution*: Update library path or install missing libraries

.. code-block:: bash

   # Check library dependencies
   ldd bin/fix-fasttrade
   
   # Update library path
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   
   # Or update system library cache
   sudo ldconfig

Configuration Errors
~~~~~~~~~~~~~~~~~~~~

**"XML parsing failed"**

.. code-block:: text

   XML parsing failed at line 15: mismatched tag

*Solution*: Validate and fix XML syntax

.. code-block:: bash

   # Validate XML syntax
   xmllint --noout config/fix-config.xml
   
   # Check for common issues
   grep -n "<.*>" config/fix-config.xml | grep -v "<.*>.*</.*>"

**"Required field missing"**

.. code-block:: text

   Configuration error: Required field 'senderCompID' not found

*Solution*: Add missing required fields to configuration

.. code-block:: xml

   <session>
       <senderCompID>YOUR_SENDER_ID</senderCompID>
       <targetCompID>YOUR_TARGET_ID</targetCompID>
       <host>your.fix.server.com</host>
       <port>9878</port>
   </session>

Performance Issues
------------------

High Latency
~~~~~~~~~~~~

**Symptoms**
* P99 latency > 10μs
* Message processing rate < 100,000 msg/sec
* High CPU wait time

**Diagnostic Steps**

.. code-block:: bash

   # Check CPU frequency scaling
   cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq
   
   # Monitor context switches
   vmstat 1
   
   # Check for memory swapping
   free -h && cat /proc/swaps

**Solutions**

.. code-block:: bash

   # Set CPU governor to performance
   echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
   
   # Enable CPU affinity
   ./bin/fix-fasttrade --config config/fix-config.xml --cpu-main 0 --cpu-fix 1
   
   # Lock memory pages
   ./bin/fix-fasttrade --config config/fix-config.xml --memory-lock

High Memory Usage
~~~~~~~~~~~~~~~~~

**Symptoms**
* Memory usage > 2GB
* Frequent garbage collection
* Out of memory errors

**Diagnostic Steps**

.. code-block:: bash

   # Check memory usage
   ps -o pid,ppid,cmd,%mem --sort=-%mem -C fix-fasttrade
   
   # Monitor memory allocation
   valgrind --tool=massif ./bin/fix-fasttrade --config config/test.xml

**Solutions**

.. code-block:: bash

   # Configure memory pools
   # Add to configuration:
   <performance>
       <memoryPoolSize>1048576</memoryPoolSize>
       <preallocateMemory>true</preallocateMemory>
   </performance>

Network Issues
--------------

Connection Timeouts
~~~~~~~~~~~~~~~~~~~

**Symptoms**
* Connection attempts fail
* Frequent disconnections
* Heartbeat timeouts

**Diagnostic Steps**

.. code-block:: bash

   # Test network connectivity
   ping fix.exchange.com
   telnet fix.exchange.com 9878
   
   # Check network latency
   mtr fix.exchange.com

**Solutions**

.. code-block:: bash

   # Increase timeout values
   <session>
       <connectTimeout>60</connectTimeout>
       <heartBtInt>60</heartBtInt>
   </session>
   
   # Optimize network settings
   echo 'net.ipv4.tcp_keepalive_time = 600' >> /etc/sysctl.conf
   sudo sysctl -p

Message Loss
~~~~~~~~~~~~

**Symptoms**
* Missing sequence numbers
* Gap fill requests
* Message replay required

**Diagnostic Steps**

.. code-block:: bash

   # Check network statistics
   netstat -i
   ss -i dst :9878
   
   # Monitor packet loss
   tcpdump -i eth0 port 9878

**Solutions**

.. code-block:: bash

   # Increase buffer sizes
   echo 'net.core.rmem_max = 134217728' >> /etc/sysctl.conf
   echo 'net.core.wmem_max = 134217728' >> /etc/sysctl.conf
   sudo sysctl -p
   
   # Enable message persistence
   <session>
       <persistSequenceNumbers>true</persistSequenceNumbers>
   </session>

Debugging Tools
---------------

Built-in Debugging
~~~~~~~~~~~~~~~~~~

Enable debug logging:

.. code-block:: xml

   <logging>
       <level>DEBUG</level>
       <console>true</console>
       <components>
           <fix>DEBUG</fix>
           <trading>DEBUG</trading>
           <network>DEBUG</network>
       </components>
   </logging>

External Tools
~~~~~~~~~~~~~~

**System Monitoring**

.. code-block:: bash

   # CPU and memory monitoring
   htop
   
   # Network monitoring
   iftop -i eth0
   
   # I/O monitoring
   iotop

**Profiling Tools**

.. code-block:: bash

   # CPU profiling
   perf record -g ./bin/fix-fasttrade --config config/fix-config.xml
   perf report
   
   # Memory profiling
   valgrind --tool=memcheck ./bin/fix-fasttrade --config config/test.xml

**Network Analysis**

.. code-block:: bash

   # Packet capture
   tcpdump -i eth0 -w fix-traffic.pcap port 9878
   
   # Analyze with Wireshark
   wireshark fix-traffic.pcap

Getting Help
------------

Self-Help Resources
~~~~~~~~~~~~~~~~~~~

1. Check the logs first: ``tail -f logs/fix-fasttrade.log``
2. Validate configuration: ``xmllint --noout config/fix-config.xml``
3. Test with minimal configuration
4. Review the :doc:``../api/index`` documentation
5. Check :doc:``../../user-guide/performance-tuning`` guide

Reporting Issues
~~~~~~~~~~~~~~~~

When reporting issues, include:

1. **System Information**
   
   .. code-block:: bash
   
    uname -a
    gcc --version
    cmake --version

2. **Build Information**
   
   .. code-block:: bash
   
    cmake .. 2>&1 | tee cmake.log
    make VERBOSE=1 2>&1 | tee build.log

3. **Runtime Information**
   
   .. code-block:: bash
   
    ./bin/fix-fasttrade --config config/fix-config.xml --log-level DEBUG 2>&1 | tee runtime.log

4. **Configuration Files**
   - Sanitized configuration files (remove sensitive information)
   - Log files showing the error
   - Steps to reproduce the issue

Support Channels
~~~~~~~~~~~~~~~~

* **GitHub Issues**: For bugs and feature requests
* **Documentation**: Check this troubleshooting guide first
* **Community**: Discussion forums and community support
* **Commercial Support**: Available for production deployments

Remember to sanitize any sensitive information (passwords, API keys, company IDs) before sharing logs or configurations.