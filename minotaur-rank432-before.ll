define i8 @cut(i64 %n4, i8 %n5, i16 %n6) {
  %n0 = icmp ne i64 %n4, 0
  %n1 = trunc nuw i8 %n5 to i1
  %n2 = or i1 %n0, %n1
  %n3 = zext i1 %n2 to i8
  ret i8 %n3
}
