define i1 @cut(i64 %n5, i16 %n6) {
  %r = icmp ugt i64 %n5, 576460752303423487
  ret i1 %r
}
