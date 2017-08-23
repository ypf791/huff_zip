CXX := g++
CFLAGS := -O3
HELPERS := get_asc file_cmp easy_data athird_data

.PHONY: $(HELPERS)
easy_data athird_data: CFLAGS += -D_DATA
get_asc: get_asc.o
file_cmp: file_cmp.o
easy_data: easy_data.o
athird_data: athird_data.o

$(HELPERS): $(addsuffix .o,$@) ../huff_code.o
	@echo Constructing $@...; \
	$(CXX) $(CFLAGS) $^ -o ../$@; \
	rm -f $^

%.o: %.cpp ../huff_code.h
	@$(CXX) $(CFLAGS) -c $< -o $@

