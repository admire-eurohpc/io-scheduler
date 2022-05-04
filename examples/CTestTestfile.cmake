# CMake generated Testfile for 
# Source directory: /home/bscuser/new/io-scheduler/examples
# Build directory: /home/bscuser/new/io-scheduler/examples
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ping_test "ping" "ofi+tcp://127.0.0.1:52000")
set_tests_properties(ping_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;162;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_input_test "ADM_input" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build" "~/new/io-scheduler/build/examples")
set_tests_properties(ADM_input_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;164;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_input_test_fail "ADM_input" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build")
set_tests_properties(ADM_input_test_fail PROPERTIES  WILL_FAIL "TRUE" _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;165;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_output_test "ADM_output" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build/examples" "~/new/io-scheduler/build")
set_tests_properties(ADM_output_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;168;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_output_test_fail "ADM_output" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build/examples")
set_tests_properties(ADM_output_test_fail PROPERTIES  WILL_FAIL "TRUE" _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;169;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_inout_test "ADM_inout" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build/examples" "~/new/io-scheduler/build")
set_tests_properties(ADM_inout_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;172;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
add_test(ADM_inout_test_fail "ADM_inout" "ofi+tcp://127.0.0.1:52000" "~/new/io-scheduler/build/examples")
set_tests_properties(ADM_inout_test_fail PROPERTIES  WILL_FAIL "TRUE" _BACKTRACE_TRIPLES "/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;173;add_test;/home/bscuser/new/io-scheduler/examples/CMakeLists.txt;0;")
