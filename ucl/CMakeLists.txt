cmake_minimum_required(VERSION 3.17)

project(ucc)

add_executable(ucl
    alloc.c ast.c decl.c declchk.c dumpast.c dom.c emit.c
    error.c expr.c exprchk.c flow.c fold.c gen.c
    input.c lex.c output.c reg.c simp.c stmt.c
    stmtchk.c str.c symbol.c tranexpr.c transtmt.c type.c
    ucl.c uildasm.c vector.c x86.c x86linux.c
)
