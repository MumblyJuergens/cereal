CPMAddPackage("gh:catchorg/Catch2@3.7.1")

set(cereal_tests_headers
)

set(cereal_tests_src
    "cerealtests.cpp"
)

add_executable(cereal_tests ${cereal_tests_src} ${cereal_tests_headers})
target_link_libraries(cereal_tests PUBLIC cereal::cereal Catch2::Catch2WithMain)
target_compile_features(cereal_tests PUBLIC cxx_std_26)
MJFullCompilerWarnings(cereal_tests)

enable_testing()
add_test(cereal_tests cereal_tests)