ticc: ticc.c

test: ticc
	./test.sh

clean:
	rm -r ticc foo *.o *~ tmp* 
