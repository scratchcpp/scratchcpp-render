# stagemodel_test
add_executable(
  stagemodel_test
  stagemodel_test.cpp
)

target_link_libraries(
  stagemodel_test
  GTest::gtest_main
  GTest::gmock_main
  libscratchcpp-gui
  scratchcppgui_mocks
  qnanopainter
  ${QT_LIBS}
  Qt6::Test
)

add_test(stagemodel_test)
gtest_discover_tests(stagemodel_test)

# spritemodel_test
add_executable(
  spritemodel_test
  spritemodel_test.cpp
)

target_link_libraries(
  spritemodel_test
  GTest::gtest_main
  GTest::gmock_main
  libscratchcpp-gui
  scratchcppgui_mocks
  qnanopainter
  ${QT_LIBS}
  Qt6::Test
)

add_test(spritemodel_test)
gtest_discover_tests(spritemodel_test)
