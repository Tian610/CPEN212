heapsize = heapsize or 1476902600
h13 = assert(init(heapsize))
p1 = assert(alloc(h13, 0x200))
p2 = assert(alloc(h13, 0x80))
p1 = assert(realloc(h13, p1, 0x280))
-- p3 = assert(alloc(h13, 0x80))