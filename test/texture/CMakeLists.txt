# texture_test
add_executable(
  texture_test
  texture_test.cpp
)

target_link_libraries(
  texture_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(texture_test)
gtest_discover_tests(texture_test)

# cputexturemanager_test
add_executable(
  cputexturemanager_test
  cputexturemanager_test.cpp
)

target_link_libraries(
  cputexturemanager_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(cputexturemanager_test)
gtest_discover_tests(cputexturemanager_test)
