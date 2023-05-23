source_filename = "main"

define i32 @main() {
entry:
  %i = alloca i32
  %a = alloca i32
  %b = alloca i32
  %arr = alloca [4 x [5 x i32]]
  %c = alloca i32
  %d = alloca i32
  %e = alloca float
  store float 2.000000e+00, float* %e
  store i32 1, i32* %d
  store i32 0, i32* %c
  store i32 0, i32* %b
  store i32 0, i32* %a
  store i32 0, i32* %i
  %0 = load i32, i32* %a
  %1 = load i32, i32* %b
  %addtmp = add i32 %0, %1
  %2 = trunc i32 %addtmp to i32*
  store i32* %2, i32* %c
  %3 = load i32, i32* %c
  ret i32 %3
}

define float @f(i32 %a) {
entry:
  %b = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  store i32 0, i32* %b
  ret float 1.000000e+00
}