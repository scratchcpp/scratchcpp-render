add_executable(
  penstate_test
  penstate_test.cpp
)

target_link_libraries(
  penstate_test
  GTest::gtest_main
  scratchcpp-render
  qnanopainter
)

add_test(penstate_test)
gtest_discover_tests(penstate_test)
