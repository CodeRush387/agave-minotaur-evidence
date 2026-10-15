define i1 @cut(i64 %n4, i16 %n5) {
  %r = icmp ugt i64 %n4, 1152921504606846975
  ret i1 %r
}
