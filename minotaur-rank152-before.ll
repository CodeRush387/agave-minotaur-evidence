define i1 @cut(i64 %n8, i64 %n9, i32 %n10, i16 %n11) {
  %n0 = trunc nuw i64 %n8 to i1
  %n1 = select i1 %n0, i64 0, i64 %n9
  %n2 = icmp eq i64 %n1, 1
  %n3 = icmp ugt i64 %n1, 1
  %n4 = icmp ne i32 %n10, 0
  %n5 = xor i1 %n0, true
  %n6 = select i1 %n5, i1 %n4, i1 false
  %n7 = select i1 %n2, i1 %n6, i1 %n3
  ret i1 %n7
}
