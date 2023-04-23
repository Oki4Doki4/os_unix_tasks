#!/bin/bash
echo "test 1337" > test.txt
echo -e "PID  lock  unlock" > result.txt
for i in {1..10}
do
  ./main test.txt -s 1 &
done  # пускаем 10 задач в параллель

sleep 5m

echo "removing tests files"
killall -SIGINT main
rm -f test.txt
