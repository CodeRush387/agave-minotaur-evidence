define i64 @cut(i64 %n4, i1 %n5, i16 %n6) {
entry:
  switch i16 %n6, label %sink [ i16 23, label %b0
                                 i16 24, label %b1 ]
b0:
  br label %join
b1:
  br i1 %n5, label %join, label %b2
b2:
  br label %join
join:
  %n0 = phi i64 [ %n4, %b1 ], [ %n4, %b2 ], [ 0, %b0 ]
  %n1 = lshr i64 %n0, 1
  %r = sub i64 %n0, %n1
  ret i64 %r
sink:
  unreachable
}
