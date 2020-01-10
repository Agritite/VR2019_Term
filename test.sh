#! /usr/bin/env bash
for i in {1..5};
do
	./watercolor ./test/in$i.jpg ./test/out$i.jpg
done
