D=db
S=server
C=client

all:$(S) $(C)

$(S):$(D).c $(S).c
	gcc $^ -o $@ -lmariadb

$(C):$(C).c
	gcc $^ -o $@

clean:
	rm -f $(S) $(C)
