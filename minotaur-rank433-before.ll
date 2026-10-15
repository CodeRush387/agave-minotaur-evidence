define i1 @cut(i64 %n3, i16 %n4) {
  %n0 = add nsw i64 %n3, -1
  %n1 = lshr i64 %n0, 1
  %n2 = icmp eq i64 %n1, %n3
  ret i1 %n2
}
