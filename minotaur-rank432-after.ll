define i8 @cut(i64 %n4, i8 %n5, i16 %n6) {
  %n0 = icmp ne i64 %n4, 0
  %r = select i1 %n0, i8 1, i8 %n5
  ret i8 %r
}
