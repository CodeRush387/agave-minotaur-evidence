define i1 @cut(i64 %n3, i16 %n4) {
  %s = lshr i64 %n3, 59
  %r = trunc i64 %s to i1
  ret i1 %r
}
