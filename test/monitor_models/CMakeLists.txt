add_executable(
  monitormodel_test
  monitormodel_test.cpp
)

target_link_libraries(
  monitormodel_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
  Qt6::Test
)

add_test(monitormodel_test)
gtest_discover_tests(monitormodel_test)

add_executable(
  valuemonitormodel_test
  valuemonitormodel_test.cpp
)

target_link_libraries(
  valuemonitormodel_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  Qt6::Test
)

add_test(valuemonitormodel_test)
gtest_discover_tests(valuemonitormodel_test)
