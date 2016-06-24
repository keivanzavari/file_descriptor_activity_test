# file_descriptor_activity_test

1. Compile package

   ```sh
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=../install
   make install
   cd ..
   ```

2. Run `file_descriptor_activity_test.ops`

   ```sh
   export RTT_COMPONENT_PATH=$(pwd)/install/lib/orocos:$RTT_COMPONENT_PATH
   deployer -l info file_descriptor_activity_test.ops
   ```

3. From a different shell/tab:
  
   ```sh
   echo "Hello World" | socat - udp:127.0.0.1:20000
   ```
   
   You should see a log line in the deployer session, like:
   ```
   9.108 [ Info   ][Logger] updateHook(): Received "Hello World"
   ```
