add_executable(
  penlayerpainter_test
  penlayerpainter_test.cpp
)

target_link_libraries(
  penlayerpainter_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  qnanopainter
)

add_test(penlayerpainter_test)
gtest_discover_tests(penlayerpainter_test)
