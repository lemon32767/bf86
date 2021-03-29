#!/bin/sh
make || exit 1
fail () {
  echo "TEST FAILED"
  exit 1
}
for f in test/*.bf; do
  echo RUNNING $f
  echo ~~~
  ./bf86 < "$f" > "$f".bin
  [ $? -eq 0 ] || fail
  chmod +x "$f".bin
  if [ -f "$f".stdin ]; then
    ./"$f".bin < "$f".stdin | tee "$f".tmpout && cmp -s "$f".tmpout "$f".stdout
    [ $? -eq 0 ] || fail
  else
    ./"$f".bin | tee "$f".tmpout && cmp -s "$f".tmpout "$f".stdout
    [ $? -eq 0 ] || fail
  fi
  echo ~~~
done

echo SUCCEED
