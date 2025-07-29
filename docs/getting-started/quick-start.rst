Quick Start Guide
=================

This guide will help you get FIX-FastTrade up and running quickly.

Prerequisites
-------------

Before you begin, ensure you have:

* **C++17** compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
* **CMake** >= 3.14
* **Boost** >= 1.71.0
* **QuickFIX** library
* **Git** for cloning the repository

Quick Installation
------------------

1. **Clone the Repository**

   .. code-block:: bash

    git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
    cd FIX-FastTrade

2. **Install Dependencies** (Optional - automated setup)

   .. code-block:: bash

    ./scripts/setup.sh

3. **Build the System**

   .. code-block:: bash

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)  # Linux: nproc, macOS: sysctl -n hw.ncpu

4. **Verify Installation**

   .. code-block:: bash

    ./bin/fix-fasttrade --help

5. **Run the Application**

   .. code-block:: bash

    ./bin/fix-fasttrade --config ../config/fix-config.xml

First Steps
-----------

Configuration
~~~~~~~~~~~~~

1. Copy the default configuration:

   .. code-block:: bash

    cp config/fix-config.xml config/my-config.xml

2. Edit the configuration file to match your environment:

   .. code-block:: xml

    <fix>
    <session>
    <senderCompID>YOUR_SENDER_ID</senderCompID>
    <targetCompID>YOUR_TARGET_ID</targetCompID>
    <host>your.fix.server.com</host>
    <port>9878</port>
    </session>
    </fix>

Basic Usage
~~~~~~~~~~~

Start the FIX-FastTrade system:

.. code-block:: bash

   # Basic usage
   ./bin/fix-fasttrade --config config/my-config.xml
   
   # High-performance mode with CPU affinity
   ./bin/fix-fasttrade \
     --config config/my-config.xml \
     --cpu-main 0 --cpu-fix 1 --cpu-order 2 \
     --memory-lock --priority -10

Performance Monitoring
~~~~~~~~~~~~~~~~~~~~~~

View real-time statistics:

.. code-block:: bash

   # Statistics printed every 30 seconds
   ./bin/fix-fasttrade --config config/my-config.xml --stats-interval 30

Expected output:

.. code-block:: text

   =====================================================
   FIX-FastTrade High-Performance Trading System v2.0.0
   =====================================================
   
   [2025-07-28] [info] Platform: macOS ARM64
   [2025-07-28] [info] Session created: FIX_SESSION_001
   [2025-07-28] [info] Connected! System is running...
   
   === Performance Statistics ===
   Uptime: 60 seconds
   Messages processed: 1,250,000
   Message rate: 20,833 msg/sec
   Order rate: 15,000 orders/sec

Next Steps
----------

* Read the :doc:``../user-guide/basic-usage`` for detailed usage instructions
* Check the :doc:``../user-guide/configuration-guide`` for advanced configuration
* See :doc:``../user-guide/performance-tuning`` for optimization tips
* Review :doc:``../reference/troubleshooting/index`` if you encounter issues

Docker Quick Start
------------------

If you prefer using Docker:

.. code-block:: bash

   # Build Docker image
   docker build -t fix-fasttrade .
   
   # Run container
   docker run -it fix-fasttrade

Integration Testing
-------------------

Verify your installation with the integration test:

.. code-block:: bash

   # Run comprehensive integration tests
   ./final_verification.sh

Expected output:

.. code-block:: text

   🚀 FIX-FastTrade v2.0.0 - Final Verification
   =============================================
   
   ✅ CMake configuration successful
   ✅ Main targets built successfully
   ✅ Application starts successfully
   ✅ FIX session creation working
   ✅ All integration tests passed!

Troubleshooting
---------------

If you encounter issues:

1. Check the :doc:``../reference/troubleshooting/index`` section
2. Verify all dependencies are installed correctly
3. Ensure your compiler supports C++17
4. Check that all required ports are available

For platform-specific issues, see:

* :doc:``../reference/troubleshooting/linux-troubleshooting``
* :doc:``../reference/troubleshooting/macos-troubleshooting``  
* :doc:``../reference/troubleshooting/windows-troubleshooting``