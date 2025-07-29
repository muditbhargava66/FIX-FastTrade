Deployment Guide
================

This guide covers deploying FIX-FastTrade in various environments, from development to production trading systems.

Deployment Overview
-------------------

FIX-FastTrade supports multiple deployment scenarios:

* **Development**: Local development and testing
* **Staging**: Pre-production testing environment
* **Production**: Live trading environment
* **Docker**: Containerized deployment
* **Cloud**: AWS, Azure, GCP deployment

Pre-Deployment Checklist
------------------------

System Requirements
~~~~~~~~~~~~~~~~~~~

Verify system requirements before deployment:

.. code-block:: bash

   # Check system specifications
   uname -a                    # Operating system
   nproc                       # CPU cores
   free -h                     # Memory
   df -h                       # Disk space
   
   # Check compiler version
   gcc --version               # GCC
   clang++ --version          # Clang
   cmake --version            # CMake

Dependencies Verification
~~~~~~~~~~~~~~~~~~~~~~~~~

Ensure all dependencies are installed:

.. code-block:: bash

   # Check required libraries
   pkg-config --modversion boost
   pkg-config --exists quickfix && echo "QuickFIX found"
   
   # Verify library paths
   ldconfig -p | grep boost
   ldconfig -p | grep quickfix

Development Deployment
----------------------

Local Development Setup
~~~~~~~~~~~~~~~~~~~~~~~

For development and testing:

.. code-block:: bash

   # Clone and build
   git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
   cd FIX-FastTrade
   
   # Install dependencies
   ./scripts/setup.sh
   
   # Build in debug mode
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make -j$(nproc)
   
   # Run with development configuration
   ./bin/fix-fasttrade --config ../config/examples/development-config.xml

Development Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

Create a development-specific configuration:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>DEV_TRADER</senderCompID>
           <targetCompID>DEV_EXCHANGE</targetCompID>
           <host>localhost</host>
           <port>9878</port>
           <heartBtInt>60</heartBtInt>
           <resetSeqNumFlag>true</resetSeqNumFlag>
       </session>
       
       <logging>
           <level>DEBUG</level>
           <console>true</console>
           <file>logs/dev-fix-fasttrade.log</file>
       </logging>
       
       <performance>
           <cpuAffinity>false</cpuAffinity>
           <memoryLock>false</memoryLock>
       </performance>
   </fix>

Staging Deployment
------------------

Staging Environment Setup
~~~~~~~~~~~~~~~~~~~~~~~~~

Deploy to staging environment for testing:

.. code-block:: bash

   # Build for staging
   mkdir build-staging && cd build-staging
   cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
   make -j$(nproc)
   
   # Install to staging directory
   make install DESTDIR=/opt/fix-fasttrade-staging

Staging Configuration
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>STAGE_TRADER_001</senderCompID>
           <targetCompID>STAGE_EXCHANGE</targetCompID>
           <host>staging.exchange.com</host>
           <port>9878</port>
           <heartBtInt>30</heartBtInt>
           <validateMessages>true</validateMessages>
       </session>
       
       <logging>
           <level>INFO</level>
           <file>/var/log/fix-fasttrade/staging.log</file>
           <rotateDaily>true</rotateDaily>
       </logging>
       
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <cpuMain>0</cpuMain>
           <cpuFix>1</cpuFix>
           <memoryLock>true</memoryLock>
       </performance>
   </fix>

Production Deployment
---------------------

Production Build
~~~~~~~~~~~~~~~~

Build optimized version for production:

.. code-block:: bash

   # Clean build for production
   rm -rf build
   mkdir build && cd build
   
   # Configure with maximum optimization
   cmake .. \
     -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto" \
     -DENABLE_SIMD=ON
   
   # Build with all CPU cores
   make -j$(nproc)
   
   # Run tests to verify build
   make test

System Installation
~~~~~~~~~~~~~~~~~~~

Install to production system:

.. code-block:: bash

   # Create system user
   sudo useradd -r -s /bin/false -d /opt/fix-fasttrade trading
   
   # Create directories
   sudo mkdir -p /opt/fix-fasttrade/{bin,lib,config,logs,data}
   sudo mkdir -p /var/log/fix-fasttrade
   
   # Install binaries
   sudo cp bin/fix-fasttrade /opt/fix-fasttrade/bin/
   sudo cp lib/libfix-fasttrade-core.a /opt/fix-fasttrade/lib/
   
   # Install configuration
   sudo cp -r ../config/* /opt/fix-fasttrade/config/
   
   # Set permissions
   sudo chown -R trading:trading /opt/fix-fasttrade
   sudo chown -R trading:trading /var/log/fix-fasttrade
   sudo chmod +x /opt/fix-fasttrade/bin/fix-fasttrade

Production Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

Create production configuration with security and performance optimizations:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>PROD_TRADER_001</senderCompID>
           <targetCompID>EXCHANGE_PROD</targetCompID>
           <host>fix.exchange.com</host>
           <port>9878</port>
           <heartBtInt>30</heartBtInt>
           <validateMessages>true</validateMessages>
           <strictFieldValidation>true</strictFieldValidation>
           <persistSequenceNumbers>true</persistSequenceNumbers>
           <sequenceNumberFile>/opt/fix-fasttrade/data/seqnums.dat</sequenceNumberFile>
       </session>
       
       <logging>
           <level>WARN</level>
           <file>/var/log/fix-fasttrade/fix-fasttrade.log</file>
           <rotateDaily>true</rotateDaily>
           <maxFiles>30</maxFiles>
           <compressRotatedFiles>true</compressRotatedFiles>
       </logging>
       
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <cpuMain>0</cpuMain>
           <cpuFix>1</cpuFix>
           <cpuOrder>2</cpuOrder>
           <cpuMarket>3</cpuMarket>
           <memoryLock>true</memoryLock>
           <threadPriority>-15</threadPriority>
           <useRealTimeScheduling>true</useRealTimeScheduling>
       </performance>
       
       <security>
           <allowedIPs>
               <ip>10.0.1.0/24</ip>
           </allowedIPs>
           <maxMessagesPerSecond>10000</maxMessagesPerSecond>
           <maxOrdersPerSecond>1000</maxOrdersPerSecond>
       </security>
   </fix>

Service Configuration
~~~~~~~~~~~~~~~~~~~~~

Create systemd service for production:

.. code-block:: bash

   # Create service file
   sudo tee /etc/systemd/system/fix-fasttrade.service << EOF
   [Unit]
   Description=FIX-FastTrade High-Performance Trading System
   After=network.target
   
   [Service]
   Type=simple
   User=trading
   Group=trading
   ExecStart=/opt/fix-fasttrade/bin/fix-fasttrade --config /opt/fix-fasttrade/config/production.xml
   WorkingDirectory=/opt/fix-fasttrade
   Restart=always
   RestartSec=5
   StandardOutput=journal
   StandardError=journal
   
   # Security settings
   NoNewPrivileges=true
   PrivateTmp=true
   ProtectSystem=strict
   ReadWritePaths=/var/log/fix-fasttrade /opt/fix-fasttrade/data
   
   # Resource limits
   LimitNOFILE=65536
   LimitMEMLOCK=infinity
   
   [Install]
   WantedBy=multi-user.target
   EOF
   
   # Enable and start service
   sudo systemctl daemon-reload
   sudo systemctl enable fix-fasttrade
   sudo systemctl start fix-fasttrade

Docker Deployment
-----------------

Docker Build
~~~~~~~~~~~~

Build Docker image:

.. code-block:: bash

   # Build image
   docker build -t fix-fasttrade:2.0.0 .
   
   # Tag for registry
   docker tag fix-fasttrade:2.0.0 your-registry/fix-fasttrade:2.0.0

Docker Compose
~~~~~~~~~~~~~~

Create docker-compose.yml for orchestration:

.. code-block:: yaml

   version: '3.8'
   
   services:
     fix-fasttrade:
       image: fix-fasttrade:2.0.0
       container_name: fix-fasttrade-prod
       restart: unless-stopped
       
       # Resource limits
       deploy:
         resources:
           limits:
             cpus: '4'
             memory: 8G
           reservations:
             cpus: '2'
             memory: 4G
       
       # CPU affinity
       cpuset: "0-3"
       
       # Privileged mode for memory locking
       privileged: true
       
       # Network configuration
       network_mode: host
       
       # Volume mounts
       volumes:
         - ./config:/app/config:ro
         - ./logs:/app/logs
         - ./data:/app/data
       
       # Environment variables
       environment:
         - FIX_CONFIG_FILE=/app/config/production.xml
         - LOG_LEVEL=INFO
       
       # Health check
       healthcheck:
         test: ["CMD", "pgrep", "fix-fasttrade"]
         interval: 30s
         timeout: 10s
         retries: 3
         start_period: 40s

Kubernetes Deployment
~~~~~~~~~~~~~~~~~~~~~

Create Kubernetes deployment:

.. code-block:: yaml

   apiVersion: apps/v1
   kind: Deployment
   metadata:
     name: fix-fasttrade
     labels:
       app: fix-fasttrade
   spec:
     replicas: 1
     selector:
       matchLabels:
         app: fix-fasttrade
     template:
       metadata:
         labels:
           app: fix-fasttrade
       spec:
         containers:
         - name: fix-fasttrade
           image: fix-fasttrade:2.0.0
           ports:
           - containerPort: 9878
           
           # Resource requirements
           resources:
             requests:
               memory: "4Gi"
               cpu: "2"
             limits:
               memory: "8Gi"
               cpu: "4"
           
           # Environment variables
           env:
           - name: FIX_CONFIG_FILE
             value: "/app/config/production.xml"
           
           # Volume mounts
           volumeMounts:
           - name: config
             mountPath: /app/config
             readOnly: true
           - name: logs
             mountPath: /app/logs
           - name: data
             mountPath: /app/data
           
           # Health checks
           livenessProbe:
             exec:
               command:
               - pgrep
               - fix-fasttrade
             initialDelaySeconds: 30
             periodSeconds: 30
           
           readinessProbe:
             tcpSocket:
               port: 9878
             initialDelaySeconds: 10
             periodSeconds: 5
         
         volumes:
         - name: config
           configMap:
             name: fix-fasttrade-config
         - name: logs
           emptyDir: {}
         - name: data
           persistentVolumeClaim:
             claimName: fix-fasttrade-data

Cloud Deployment
----------------

AWS Deployment
~~~~~~~~~~~~~~

Deploy on AWS EC2 with optimizations:

.. code-block:: bash

   # Launch optimized EC2 instance
   aws ec2 run-instances \
     --image-id ami-0abcdef1234567890 \
     --instance-type c5n.xlarge \
     --key-name your-key-pair \
     --security-group-ids sg-12345678 \
     --subnet-id subnet-12345678 \
     --placement AvailabilityZone=us-east-1a,Tenancy=dedicated \
     --ebs-optimized \
     --monitoring Enabled=true

Instance optimization:

.. code-block:: bash

   # Install on EC2 instance
   sudo yum update -y
   sudo yum install -y gcc-c++ cmake boost-devel
   
   # Configure for low latency
   echo 'net.core.busy_read=50' >> /etc/sysctl.conf
   echo 'net.core.busy_poll=50' >> /etc/sysctl.conf
   sudo sysctl -p
   
   # Set CPU governor
   echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

Azure Deployment
~~~~~~~~~~~~~~~~

Deploy on Azure with Container Instances:

.. code-block:: bash

   # Create resource group
   az group create --name fix-fasttrade-rg --location eastus
   
   # Deploy container
   az container create \
     --resource-group fix-fasttrade-rg \
     --name fix-fasttrade \
     --image your-registry/fix-fasttrade:2.0.0 \
     --cpu 4 \
     --memory 8 \
     --ports 9878 \
     --environment-variables FIX_CONFIG_FILE=/app/config/production.xml

Monitoring and Maintenance
--------------------------

Health Monitoring
~~~~~~~~~~~~~~~~~

Set up monitoring for production deployment:

.. code-block:: bash

   # System monitoring script
   #!/bin/bash
   
   # Check if process is running
   if ! pgrep -f fix-fasttrade > /dev/null; then
       echo "CRITICAL: FIX-FastTrade process not running"
       systemctl restart fix-fasttrade
   fi
   
   # Check memory usage
   MEM_USAGE=$(ps -o pid,ppid,cmd,%mem,%cpu --sort=-%mem -C fix-fasttrade | tail -n +2 | awk '{print $4}')
   if (( $(echo "$MEM_USAGE > 80" | bc -l) )); then
       echo "WARNING: High memory usage: $MEM_USAGE%"
   fi
   
   # Check log file size
   LOG_SIZE=$(du -m /var/log/fix-fasttrade/fix-fasttrade.log | cut -f1)
   if [ "$LOG_SIZE" -gt 1000 ]; then
       echo "WARNING: Log file size: ${LOG_SIZE}MB"
   fi

Log Management
~~~~~~~~~~~~~~

Configure log rotation and archival:

.. code-block:: bash

   # Create logrotate configuration
   sudo tee /etc/logrotate.d/fix-fasttrade << EOF
   /var/log/fix-fasttrade/*.log {
       daily
       rotate 30
       compress
       delaycompress
       missingok
       notifempty
       create 644 trading trading
       postrotate
           systemctl reload fix-fasttrade
       endscript
   }
   EOF

Backup and Recovery
~~~~~~~~~~~~~~~~~~~

Set up backup procedures:

.. code-block:: bash

   # Backup script
   #!/bin/bash
   
   BACKUP_DIR="/backup/fix-fasttrade/$(date +%Y%m%d)"
   mkdir -p "$BACKUP_DIR"
   
   # Backup configuration
   cp -r /opt/fix-fasttrade/config "$BACKUP_DIR/"
   
   # Backup data files
   cp -r /opt/fix-fasttrade/data "$BACKUP_DIR/"
   
   # Backup logs (last 7 days)
   find /var/log/fix-fasttrade -name "*.log*" -mtime -7 -exec cp {} "$BACKUP_DIR/" \;
   
   # Create archive
   tar -czf "$BACKUP_DIR.tar.gz" -C /backup/fix-fasttrade "$(basename $BACKUP_DIR)"
   rm -rf "$BACKUP_DIR"

Deployment Verification
-----------------------

Post-Deployment Testing
~~~~~~~~~~~~~~~~~~~~~~~

Verify deployment success:

.. code-block:: bash

   # Check service status
   systemctl status fix-fasttrade
   
   # Verify process is running
   ps aux | grep fix-fasttrade
   
   # Check network connectivity
   netstat -tlnp | grep :9878
   
   # Test configuration
   /opt/fix-fasttrade/bin/fix-fasttrade --config /opt/fix-fasttrade/config/production.xml --validate
   
   # Monitor logs
   tail -f /var/log/fix-fasttrade/fix-fasttrade.log

Performance Validation
~~~~~~~~~~~~~~~~~~~~~~

Validate performance meets requirements:

.. code-block:: bash

   # Run performance test
   ./bin/fix-fasttrade-benchmark \
     --config config/production.xml \
     --duration 300 \
     --target-latency 1000  # nanoseconds

Security Validation
~~~~~~~~~~~~~~~~~~~

Verify security configuration:

.. code-block:: bash

   # Check file permissions
   ls -la /opt/fix-fasttrade/
   
   # Verify user isolation
   id trading
   
   # Check network access
   iptables -L | grep 9878
   
   # Validate SSL/TLS configuration
   openssl s_client -connect fix.exchange.com:9878

Rollback Procedures
-------------------

If deployment issues occur:

.. code-block:: bash

   # Stop current version
   sudo systemctl stop fix-fasttrade
   
   # Restore previous version
   sudo cp /backup/fix-fasttrade-previous/bin/fix-fasttrade /opt/fix-fasttrade/bin/
   sudo cp /backup/fix-fasttrade-previous/config/* /opt/fix-fasttrade/config/
   
   # Restart service
   sudo systemctl start fix-fasttrade
   
   # Verify rollback
   systemctl status fix-fasttrade

This deployment guide ensures a robust, secure, and high-performance deployment of FIX-FastTrade across various environments.