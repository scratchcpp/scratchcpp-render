add_executable(
  renderedtarget_test
  renderedtarget_test.cpp
)

target_link_libraries(
  renderedtarget_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  Qt6::Test
  qnanopainter
)

add_test(renderedtarget_test)
gtest_discover_tests(renderedtarget_test)
