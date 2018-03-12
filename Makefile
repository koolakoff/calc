PROG=app.out

CFLAGS :=

$(PROG): *.cpp
	g++ $(CFLAGS) *.cpp -o $@

clean:
	rm $(PROG)
