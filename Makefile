CXX := g++
CFLAGS := -O3
DEBUGFLAGS := -g -Wall
SUBTTL := ypf unypf
HELPERS := get_asc file_cmp easy_data athird_data

.PHONY: debug test helpers
ypf: MAKEFLAGS := $(CFLAGS)
ypf_test debug test: MAKEFLAGS := $(DEBUGFLAGS)

ypf debug: main.o huff_code_tree.o huff_code.o
	$(CXX) $(MAKEFLAGS) $^ -o ypf

test: main_test.o huff_code_tree.o huff_code.o
	$(CXX) $(MAKEFLAGS) $^ -o ypf_test

helpers: $(HELPERS)

%.o: %.cpp huff_code.h
	$(CXX) $(MAKEFLAGS) -c $< -o $@

$(HELPERS):
	@cd helpers; \
	make -s $@

clean: cleantest cleanhelpers
	@rm -f *.o ypf ypf_test

cleantest:
	@echo Test output files cleaning...; \
	rm -f $(addprefix *.,$(SUBTTL)); \
	rm -f $(addprefix testdata/*.,$(SUBTTL)); \
	rm -f *.t1 *.log

cleanhelpers:
	@echo Supporting subprograms cleaning...; \
	rm -f $(HELPERS)
