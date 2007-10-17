
all:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make; cd ..; fi; done;

clean:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make clean; cd ..; fi; done;
