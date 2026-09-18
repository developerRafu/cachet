BEND := $(HOME)/.bend/bin/bend
OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null || echo /usr/local/opt/openssl@3)
export PATH := $(HOME)/.bend/bin:$(PATH)

.PHONY: check proof example publish clean

check:
	$(BEND) types.bend
	$(BEND) cachet.bend
	$(BEND) claims.bend
	$(BEND) jwt.bend
	$(BEND) auth.bend
	@mkdir -p build
	$(BEND) examples/sign_verify.bend -o build/sign_verify.c

proof:
	$(BEND) PROOF.bend

example: build/sign_verify
	./build/sign_verify

build/sign_verify.c: examples/sign_verify.bend
	@mkdir -p build
	$(BEND) examples/sign_verify.bend -o build/sign_verify.c

build/sign_verify: build/sign_verify.c
	clang -std=c11 -O3 build/sign_verify.c \
		-Ieffs \
		-I$(OPENSSL_PREFIX)/include \
		-L$(OPENSSL_PREFIX)/lib \
		-lcrypto -lpthread -lm \
		-o build/sign_verify

publish:
	$(BEND) cachet.bend --publish
	$(BEND) jwt.bend --publish
	$(BEND) auth.bend --publish

clean:
	rm -rf build
