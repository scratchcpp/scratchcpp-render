add_executable(
  effecttransform_test
  effecttransform_test.cpp
)

target_link_libraries(
  effecttransform_test
  GTest::gtest_main
  scratchcpp-render
  qnanopainter
)

add_test(effecttransform_test)
gtest_discover_tests(effecttransform_test)
