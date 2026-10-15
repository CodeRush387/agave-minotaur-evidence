declare i64 @llvm.umax.i64(i64, i64)
define i1 @cut(i64 %n3, i16 %n4) {
  %n0 = call i64 @llvm.umax.i64(i64 %n3, i64 4)
  %n1 = shl i64 %n0, 4
  %n2 = icmp ugt i64 %n1, 9223372036854775800
  ret i1 %n2
}
