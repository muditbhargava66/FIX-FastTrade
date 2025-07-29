Contributing to FIX-FastTrade
=============================

We welcome contributions to FIX-FastTrade! This guide will help you get started with contributing to the project.

Getting Started
---------------

Development Environment Setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Fork and Clone**

   .. code-block:: bash

    # Fork the repository on GitHub, then clone your fork
    git clone https://github.com/YOUR_USERNAME/FIX-FastTrade.git
    cd FIX-FastTrade
      
    # Add upstream remote
    git remote add upstream https://github.com/muditbhargava66/FIX-FastTrade.git

2. **Install Dependencies**

   .. code-block:: bash

    # Install development dependencies
    ./scripts/setup.sh
      
    # Install additional development tools
    # Ubuntu/Debian
    sudo apt install clang-format clang-tidy doxygen
      
    # macOS
    brew install clang-format doxygen

3. **Build and Test**

   .. code-block:: bash

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)
      
    # Run tests
    make test

Development Workflow
--------------------

Branch Strategy
~~~~~~~~~~~~~~~

We use a Git flow-based branching strategy:

* **main**: Production-ready code
* **develop**: Integration branch for features
* **feature/**: Feature development branches
* **hotfix/**: Critical bug fixes
* **release/**: Release preparation branches

Creating a Feature Branch
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Update your local repository
   git checkout develop
   git pull upstream develop
   
   # Create feature branch
   git checkout -b feature/your-feature-name
   
   # Make your changes and commit
   git add .
   git commit -m "Add your feature description"
   
   # Push to your fork
   git push origin feature/your-feature-name

Code Standards
--------------

C++ Coding Standards
~~~~~~~~~~~~~~~~~~~~

We follow modern C++17 best practices:

**Naming Conventions**

.. code-block:: cpp

   // Classes: PascalCase
   class MessageParser {
   public:
       // Methods: camelCase
       void parseMessage(const std::string& message);
       
       // Private members: trailing underscore
   private:
       std::string buffer_;
       int messageCount_;
   };
   
   // Constants: UPPER_SNAKE_CASE
   static constexpr int MAX_MESSAGE_SIZE = 8192;
   
   // Namespaces: lowercase
   namespace fix {
   namespace model {
       // ...
   }
   }

**Code Style**

.. code-block:: cpp

   // Use auto for type deduction when type is obvious
   auto message = MessageParser::parse(buffer);
   
   // Use range-based for loops
   for (const auto& order : orders) {
       processOrder(order);
   }
   
   // Use smart pointers for memory management
   std::unique_ptr<Session> session = std::make_unique<Session>(config);
   
   // Use const correctness
   const std::string& getSymbol() const { return symbol_; }
   
   // Use RAII for resource management
   class FileHandler {
   public:
       FileHandler(const std::string& filename) : file_(filename) {}
       ~FileHandler() { /* automatic cleanup */ }
   private:
       std::ofstream file_;
   };

Code Formatting
~~~~~~~~~~~~~~~

We use clang-format for consistent code formatting:

.. code-block:: bash

   # Format all source files
   find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i
   
   # Check formatting
   ./tools/scripts/check-format.sh

Static Analysis
~~~~~~~~~~~~~~~

Run static analysis before submitting:

.. code-block:: bash

   # Run clang-tidy
   clang-tidy src/**/*.cpp -- -Iinclude -std=c++17
   
   # Run cppcheck
   cppcheck --enable=all --std=c++17 src/ include/

Testing Guidelines
------------------

Test Structure
~~~~~~~~~~~~~~

We use Google Test for unit testing:

.. code-block:: cpp

   #include <gtest/gtest.h>
   #include "fix/MessageParser.h"
   
   class MessageParserTest : public ::testing::Test {
   protected:
       void SetUp() override {
           // Test setup
       }
       
       void TearDown() override {
           // Test cleanup
       }
   };
   
   TEST_F(MessageParserTest, ParseValidMessage) {
       // Arrange
       std::string fixMessage = "8=FIX.4.4|9=148|35=D|...";
       
       // Act
       auto message = MessageParser::parse(fixMessage);
       
       // Assert
       EXPECT_EQ(message.getField(35), "D");
       EXPECT_EQ(message.getField(8), "FIX.4.4");
   }

Test Coverage
~~~~~~~~~~~~~

Maintain high test coverage:

.. code-block:: bash

   # Build with coverage
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
   make -j$(nproc)
   
   # Run tests
   make test
   
   # Generate coverage report
   make coverage

Performance Testing
~~~~~~~~~~~~~~~~~~~

For performance-critical code, include benchmarks:

.. code-block:: cpp

   #include <benchmark/benchmark.h>
   #include "fix/MessageParser.h"
   
   static void BM_MessageParsing(benchmark::State& state) {
       std::string fixMessage = "8=FIX.4.4|9=148|35=D|...";
       
       for (auto _ : state) {
           auto message = MessageParser::parse(fixMessage);
           benchmark::DoNotOptimize(message);
       }
   }
   BENCHMARK(BM_MessageParsing);

Documentation
-------------

Code Documentation
~~~~~~~~~~~~~~~~~~

Use Doxygen-style comments for API documentation:

.. code-block:: cpp

   /**
    * @brief Parses a FIX message string into a Message object
    * 
    * This function performs zero-copy parsing when possible to minimize
    * latency in high-frequency trading scenarios.
    * 
    * @param messageString The raw FIX message string
    * @return Parsed Message object
    * @throws std::invalid_argument if message format is invalid
    * @throws std::runtime_error if parsing fails
    * 
    * @example
    * @code
    * std::string fixMsg = "8=FIX.4.4|9=148|35=D|...";
    * auto message = MessageParser::parse(fixMsg);
    * std::string msgType = message.getField(35);
    * @endcode
    */
   static Message parse(const std::string& messageString);

User Documentation
~~~~~~~~~~~~~~~~~~

Update relevant documentation when making changes:

* **API changes**: Update ``docs/reference/api/``
* **Configuration changes**: Update ``docs/getting-started/configuration.rst``
* **New features**: Update ``docs/user-guide/``
* **Bug fixes**: Update ``CHANGELOG.md``

Commit Guidelines
-----------------

Commit Message Format
~~~~~~~~~~~~~~~~~~~~~

Use conventional commit format:

.. code-block:: text

   <type>(<scope>): <description>
   
   [optional body]
   
   [optional footer(s)]

**Types:**
* ``feat``: New feature
* ``fix``: Bug fix
* ``docs``: Documentation changes
* ``style``: Code style changes (formatting, etc.)
* ``refactor``: Code refactoring
* ``perf``: Performance improvements
* ``test``: Adding or updating tests
* ``chore``: Maintenance tasks

**Examples:**

.. code-block:: text

   feat(fix): add zero-copy message parsing
   
   Implement zero-copy parsing for FIX messages to reduce latency
   in high-frequency trading scenarios. This change improves
   message processing performance by 40%.
   
   Closes #123

.. code-block:: text

   fix(session): resolve memory leak in session cleanup
   
   Fixed memory leak that occurred when sessions were destroyed
   without proper cleanup of message buffers.
   
   Fixes #456

Pull Request Process
--------------------

Before Submitting
~~~~~~~~~~~~~~~~~

1. **Ensure tests pass**

   .. code-block:: bash

    make test

2. **Run static analysis**

   .. code-block:: bash

    clang-tidy src/**/*.cpp
    cppcheck --enable=all src/ include/

3. **Check code formatting**

   .. code-block:: bash

    clang-format -i src/**/*.cpp include/**/*.h

4. **Update documentation**

   .. code-block:: bash

    # Generate API docs
    make docs

5. **Update CHANGELOG.md** if applicable

Pull Request Template
~~~~~~~~~~~~~~~~~~~~~

When creating a pull request, include:

.. code-block:: text

   ## Description
   Brief description of changes made.
   
   ## Type of Change
   - [ ] Bug fix (non-breaking change which fixes an issue)
   - [ ] New feature (non-breaking change which adds functionality)
   - [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
   - [ ] Documentation update
   
   ## Testing
   - [ ] Unit tests pass
   - [ ] Integration tests pass
   - [ ] Performance tests pass (if applicable)
   - [ ] Manual testing completed
   
   ## Checklist
   - [ ] Code follows project style guidelines
   - [ ] Self-review completed
   - [ ] Code is commented, particularly in hard-to-understand areas
   - [ ] Documentation updated
   - [ ] No new warnings introduced

Review Process
~~~~~~~~~~~~~~

1. **Automated Checks**: CI/CD pipeline runs tests and static analysis
2. **Code Review**: Maintainers review code for quality and design
3. **Testing**: Reviewers may test changes locally
4. **Approval**: At least one maintainer approval required
5. **Merge**: Squash and merge to maintain clean history

Issue Reporting
---------------

Bug Reports
~~~~~~~~~~~

When reporting bugs, include:

1. **System Information**
   
   .. code-block:: bash
   
    uname -a
    gcc --version
    cmake --version

2. **Steps to Reproduce**
   
   .. code-block:: text
   
    1. Configure system with...
    2. Run command...
    3. Observe error...

3. **Expected vs Actual Behavior**

4. **Logs and Error Messages**

5. **Configuration Files** (sanitized)

Feature Requests
~~~~~~~~~~~~~~~~

For feature requests, provide:

1. **Use Case**: Why is this feature needed?
2. **Proposed Solution**: How should it work?
3. **Alternatives**: What alternatives have you considered?
4. **Additional Context**: Any other relevant information

Community Guidelines
--------------------

Code of Conduct
~~~~~~~~~~~~~~~

* Be respectful and inclusive
* Focus on constructive feedback
* Help others learn and grow
* Maintain professional communication

Getting Help
~~~~~~~~~~~~

* **Documentation**: Check docs first
* **GitHub Issues**: Search existing issues
* **Discussions**: Use GitHub Discussions for questions
* **Code Review**: Ask for feedback on complex changes

Recognition
~~~~~~~~~~~

Contributors are recognized in:

* **CONTRIBUTORS.md**: All contributors listed
* **Release Notes**: Major contributions highlighted
* **GitHub**: Contributor statistics and badges

Thank you for contributing to FIX-FastTrade! Your contributions help make high-performance trading technology accessible to everyone.