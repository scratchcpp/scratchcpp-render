add_executable(
  projectloader_test
  projectloader_test.cpp
)

target_link_libraries(
  projectloader_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  Qt6::Test
)

add_test(projectloader_test)
gtest_discover_tests(projectloader_test)
