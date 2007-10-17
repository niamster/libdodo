
all:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make; cd ..; fi; done;

test:
	for i in ./*; do if [ -d $$i ]; then cd $$i; echo "execution of " `pwd`; ./test; cd ..; fi; done;
	
clean:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make clean; cd ..; fi; done;
