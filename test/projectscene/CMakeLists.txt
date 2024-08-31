add_executable(
  projectscene_test
  projectscene_test.cpp
)

target_link_libraries(
  projectscene_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  Qt6::Test
)

add_test(projectscene_test)
gtest_discover_tests(projectscene_test)
