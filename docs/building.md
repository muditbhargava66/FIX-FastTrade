# Building

To ensure that everything is working and building correctly in your FIX-FastTrade project, including the main repository, Google Test, and QuickFIX, you can follow these steps in the terminal:

1. Clone the FIX-FastTrade repository:
   ```
   git clone https://github.com/muditbhargava66/FIX-FastTrade.git
   cd FIX-FastTrade
   ```

2. Build and install QuickFIX:
   ```
   cd third_party/quickfix
   mkdir build
   cd build
   cmake ..
   make
   sudo make install
   cd ../../../
   ```

3. Build and install Google Test (optional, if running tests):
   ```
   cd third_party/googletest
   mkdir build
   cd build
   cmake ..
   make
   sudo make install
   cd ../../../
   ```

4. Create a build directory for the FIX-FastTrade project:
   ```
   mkdir build
   cd build
   ```

5. Generate the build files using CMake:
   ```
   cmake ..
   ```

6. Build the FIX-FastTrade project:
   ```
   make
   ```

7. Run the tests (optional):
   ```
   ctest
   ```

8. Start the FIX-FastTrade executable:
   ```
   ./bin/fix-fasttrade
   ```

9. Monitor the terminal for any errors or issues during the build and execution process.

If any errors occur during the build process, review the error messages and ensure that all dependencies are properly installed and configured. Double-check the CMake configuration and make sure all required libraries and headers are available.

If the tests fail, investigate the specific test cases and see if there are any issues with the code or the test environment. Make sure the tests are up to date with the latest changes in the codebase.

If the FIX-FastTrade executable fails to start or encounters runtime errors, review the error messages and logs to identify the cause of the issue. Check the configuration files and ensure that all required settings are properly defined.

After successfully building and running the FIX-FastTrade system, you can proceed to test its functionality by sending FIX messages and verifying the expected behavior. Use a FIX client or simulator to connect to the FIX-FastTrade system and send various types of messages to test different scenarios.

Remember to regularly update your local repository with the latest changes from the remote repository to ensure you have the most up-to-date codebase:
```
git pull origin main
```

If you encounter any issues or have questions, consult the project's documentation, seek help from the community or the project maintainers, and refer to the troubleshooting guides or FAQs.

By following these steps and regularly monitoring the build and execution process, you can ensure that everything is working and building correctly in your FIX-FastTrade project, including the main repository, Google Test, and QuickFIX.

---