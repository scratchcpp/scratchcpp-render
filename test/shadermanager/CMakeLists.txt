add_executable(
  shadermanager_test
  shadermanager_test.cpp
)

target_link_libraries(
  shadermanager_test
  GTest::gtest_main
  scratchcpp-render
  qnanopainter
)

add_test(shadermanager_test)
gtest_discover_tests(shadermanager_test)
