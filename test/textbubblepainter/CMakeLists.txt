add_executable(
  textbubblepainter_test
  textbubblepainter_test.cpp
)

target_link_libraries(
  textbubblepainter_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
  qnanopainter
)

add_test(textbubblepainter_test)
gtest_discover_tests(textbubblepainter_test)
