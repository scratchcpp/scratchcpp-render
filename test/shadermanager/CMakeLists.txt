add_executable(
  shadermanager_test
  shadermanager_test.cpp
)

target_link_libraries(
  shadermanager_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp-render
  scratchcpprender_mocks
  qnanopainter
  ${QT_LIBS}
)

add_test(shadermanager_test)
gtest_discover_tests(shadermanager_test)
