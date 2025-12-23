include_directories(../)

list(APPEND TestList
    "lexer_source_tests"
    "lexer_tests"
)

foreach (Test IN LISTS TestList)
    add_executable("veebee_${Test}" "${Test}.c")
    add_test("veebee_${Test}" "veebee_${Test}")
    target_link_libraries("veebee_${Test}" veebee_backend)
endforeach ()
