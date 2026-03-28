add_executable(
  penlayer_test
  penlayer_test.cpp
)

target_link_libraries(
  penlayer_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp-render
  scratchcpprender_mocks
  qnanopainter
  ${QT_LIBS}
  Qt6::Test
)

add_test(penlayer_test)
gtest_discover_tests(penlayer_test)
