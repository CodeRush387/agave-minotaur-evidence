define i1 @cut(i64 %n4, i16 %n5) {
  %n0 = shl i64 %n4, 3
  %n1 = icmp ugt i64 %n4, 2305843009213693951
  %n2 = icmp ugt i64 %n0, 9223372036854775800
  %n3 = or i1 %n1, %n2
  ret i1 %n3
}
