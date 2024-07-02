# QuickFIX Installation on macOS

This document provides detailed steps for installing QuickFIX on macOS.

## Prerequisites

- Install `boost` using Homebrew: `brew install boost`
- Install `openssl` using Homebrew: `brew install openssl`

## Installation Steps

1. **Clone the QuickFIX repository**

```bash
git clone https://github.com/quickfix/quickfix.git
```

2. **Navigate to the QuickFIX directory**

```bash
cd quickfix
```

3. **Create a build directory**

```bash
mkdir build
cd build
```

4. **Configure CMake**

```bash
cmake ..
```

5. **Build QuickFIX**

```bash
cmake --build .
```

6. **Install QuickFIX**

```bash
cmake --install .
```

7. **Set the installation directory**

By default, QuickFIX will be installed in `/usr/local/`. If you want to install it in a different location, such as your home directory, you can specify the installation prefix during the CMake configuration step:

```bash
cd ..
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/quickfix-install
cmake --build .
cmake --install .
```

This will install QuickFIX in `$HOME/quickfix-install`.

8. **Update the system's PATH**

If you installed QuickFIX in a custom location, update your system's `PATH` to include the QuickFIX `bin` directory:

```bash
export PATH=$PATH:$HOME/quickfix-install/bin
```

9. **Verify the installation**

You can verify the installation by running one of the following commands:

- Check the QuickFIX version using the `quickfix-config` executable:

```bash
quickfix-config --version
```

- Run one of the example executables like `executor` or `ordermatch`:

```bash
$HOME/quickfix-install/bin/executor
$HOME/quickfix-install/bin/ordermatch
```

These executables should print the QuickFIX version information when run without any arguments.

- Check the version information in the `CMakeLists.txt` file:

```bash
grep 'set(PACKAGE_VERSION ' $HOME/quickfix-install/CMakeLists.txt
```

If you encounter any issues during the installation process, please refer to the official QuickFIX documentation or seek assistance from the QuickFIX community.

---