define i1 @cut(i64 %n6, i16 %n7) {
  %n0 = shl i64 %n6, 3
  %n1 = add i64 %n0, 23
  %n2 = and i64 %n1, -16
  %n3 = add i64 %n6, 17
  %n4 = add i64 %n3, %n2
  %n5 = icmp eq i64 %n4, 0
  ret i1 %n5
}
