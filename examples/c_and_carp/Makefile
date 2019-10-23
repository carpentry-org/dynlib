app=example.carp
lib_files=libt.so

lib: $(lib_files)

lib%.so: %.c
	$(CC) -shared -fPIC -o $@ $<

app:
	carp -x $(app)
