all : writer_prefer fair_reader_writer

CC = gcc

writer_prefer: writer_prefer.c
	$(CC) -o $@ $< -pthread

fair_reader_writer: fair_reader_writer.c
	$(CC) -o $@ $< -pthread

clean:
	rm -f *.o
	rm -f *.out
	rm -f writer_prefer
	rm -f fair_reader_writer