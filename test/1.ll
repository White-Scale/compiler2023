; ModuleID = 'main'
source_filename = "main"

declare i32 @putchar(i8)

define i32 @f(i32 %a) {
entry:
  %a1 = alloca i32
  store i32 %a, i32* %a1
  %0 = load i32, i32* %a1
  %eqtmp = icmp eq i32 %0, 0
  %1 = sext i1 %eqtmp to i32
  %ifcond = icmp ne i32 %1, 0
  br i1 %ifcond, label %then, label %else2

then:                                             ; preds = %entry
  ret i32 1

else2:                                            ; preds = %entry
  %2 = load i32, i32* %a1
  %3 = load i32, i32* %a1
  %subtmp = sub i32 %3, 1
  %calltmp = call i32 @f(i32 %subtmp)
  %multmp = mul i32 %2, %calltmp
  ret i32 %multmp

ifcont3:                                          ; preds = <badref>, <badref>
  ret i32 undef
}

define i32 @main() {
entry:
  %a = alloca i32
  %b = alloca float
  %c = alloca i8
  %arr = alloca [4 x [5 x i8]]
  store i8 72, i8* %c
  store float 1.000000e+00, float* %b
  store i32 0, i32* %a
  %0 = load i8, i8* %c
  %calltmp = call i32 @putchar(i8 %0)
  store i8 101, i8* %c
  %1 = load i8, i8* %c
  %calltmp1 = call i32 @putchar(i8 %1)
  store i8 108, i8* %c
  %2 = load i8, i8* %c
  %calltmp2 = call i32 @putchar(i8 %2)
  %3 = load i8, i8* %c
  %calltmp3 = call i32 @putchar(i8 %3)
  store i8 111, i8* %c
  %4 = load i8, i8* %c
  %calltmp4 = call i32 @putchar(i8 %4)
  store i8 10, i8* %c
  %5 = load i8, i8* %c
  %calltmp5 = call i32 @putchar(i8 %5)
  %arraytmp = getelementptr [4 x [5 x i8]], [4 x [5 x i8]]* %arr, i32 0, i32 1
  %arraytmp6 = getelementptr [5 x i8], [5 x i8]* %arraytmp, i32 0, i32 1
  store i8 101, i8* %arraytmp6
  %arraytmp7 = getelementptr [4 x [5 x i8]], [4 x [5 x i8]]* %arr, i32 0, i32 1
  %arraytmp8 = getelementptr [5 x i8], [5 x i8]* %arraytmp7, i32 0, i32 1
  %arrayloadtmp = load i8, i8* %arraytmp8
  %calltmp9 = call i32 @putchar(i8 %arrayloadtmp)
  %6 = load i8, i8* %c
  %7 = sext i8 %6 to i32
  store i32 %7, i32* %a
  store i32 1, i32* %a
  store i32 1073741824, i32* %a
  %8 = load i8, i8* %c
  %9 = sext i8 %8 to i32
  store i32 %9, i32* %a
  ret i32 0
}
