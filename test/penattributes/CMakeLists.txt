add_executable(
  penattributes_test
  penattributes_test.cpp
)

target_link_libraries(
  penattributes_test
  GTest::gtest_main
  scratchcpp-render
  qnanopainter
)

add_test(penattributes_test)
gtest_discover_tests(penattributes_test)
