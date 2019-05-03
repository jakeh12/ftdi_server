all: server

server: server.c
	gcc -o server server.c
	
clean:
	rm -f server
	
test: server
	bash test_server.sh
	
deploy: all test clean
	git status
	read -r -p 'Commit message: ' DESC; \
	git add .; \
	git add -u; \
	git commit -m "$$DESC"; \
	git push origin master