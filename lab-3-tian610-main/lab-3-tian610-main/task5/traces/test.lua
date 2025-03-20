heapsize = heapsize or 30000
h31 = assert(init(heapsize))
p1 = alloc(h31,0x6506)
alloc(h31,0x10)
debug(h31,0)
assert(realloc(h31, p1, 0x650b))
debug(h31,0)
