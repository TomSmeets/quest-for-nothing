# Memory methods

# Good Idea

## Flexible arenas
Allocating memory is easy but freeing memory is difficult. I don't want to constantly think about freeing individual objects.
Memory arenas help with this.

There are fixed points in the program were I will call 'free' and everything in the given arena is freed.
This means I don't have to think about individual objects.

Some example areas are:
- Game
- Level
- Frame
- Temporary

All memory is split into blocks. And can be directly allocated and freed in big chucks from the OS.

    +---------------+
    | Memory Block  |
    | - size, next  |
    +---------------+
    .               .
    .               .
    .               .
    .     Free      .
    .               .
    .               .
    .               .
    . . . . . . . . .

```c
static mem_page *os_alloc_page(u64 size);
static void os_free_page(mem_page *page);
```

A memory arena can use a number of blocks to construct its memory.

    +---------------+              +---------------+     
    | Mem Arena     |     +---->   | Memory Block  | ---+
    +---------------+     |        +---------------+    |
    | Used          | ----+        |  Entity 3     |    |
    | Start         | ---------->  +---------------+    |
    | End           | ----+        .               .    |
    +---------------+     |        .     Free      .    |
                          |        .               .    |
                          |        .               .    |
                          |        .               .    |
                          \---->   . . . . . . . . .    |
                                                        |
    +---------------------------------------------------+
    |                  
    |   +---------------+       +---------------+    
    +-> | Memory Block  | ----> | Memory Block  | ---> ...
        +---------------+       +---------------+    
        | Entity 2      |       |               |    
        +---------------+       |               |    
        | Image         |       |               |    
        |               |       |     Some      |    
        +---------------+       |     Big       |    
        | Entity 1      |       |     File      |    
        |               |       |               |    
        +---------------+       |               |    
                                |               |    
                                |               |    
                                |               |    
                                |               |    
                                +---------------+    

A guess for a good minimum size for a block would be something like 1MB. But anything would work probably.

Memory When a block is full, the arena just allocates a new block from the OS with `os_alloc_page` and pushes it to the 'Used' stack. The start and end pointers are also updated.

To free an arena we iterate over all blocks and call `os_free_page`.

An empty memory arena will just be the single struct 0 pointers for used,start and end.
This allows for very easy creation of memory arenas. Just use `mem_arena m = {}`

To speed up the allocation and deallocation of pages we can optionally cache freed pages.
We don't have to return them to the OS directly, but hang on to them and give them to the next allocation.
Caching should probably be only done with the most common pages that are the minimum allocation size.
When freeing big pages that are used for files, we want to return that memory to the OS.

This method is very flexible. It allows us to create huge arenas that have very long lifetimes.
But I can also create very short lived arenas and use it as a very rough malloc/free.

### Flexible
An example is the `printf` function.
I need some memory to create the formatted string.
But the memory can be freed directly after the print is performed.
I don't want to pass a temporary memory arena every time to this function. Fortunately I can just use a memory page directly.
This will only use one page and return it to the page cache directly.

### Advantages
- Flexible
- Simple
- Performant

### Disadvantages
- ?

```c
mem_arena m = {};
char *out = format_string(&m, format, args...);
print(out);
mem_clear(&m);
```

# Bad Ideas (Don't use these)

## Malloc / Free
The most common memory allocation method of malloc/free is never a good idea. Flexible Arenas is strictly better.

### Disadvantages
- Complex to implement
- Slow
- Very Fragmented Memory
- I have to track the lifetime of every object

## Permanent + Frame Arena

        Permanent             Frame        
    +---------------+     +---------------+
    | Game State    |     | Draw Call     |
    |               |     |               |
    +---------------+     +---------------+
    | Level         |     | Vertex        |
    |               |     +---------------+
    |               |     | Temp Image    |
    |               |     |               |
    |               |     |               |
    +---------------+     |               |
    .               .     |               |
    .               .     +---------------+
    .               .     .               .
    . . . . . . . . .     . . . . . . . . .

### Advantages
- Very simple

### Disadvantage
- Not flexible enough

## Two Frame Arenas

     Previous Frame                    Next Frame
    +---------------+               +---------------+
    | Frame Header  |               | Frame Header  |
    | - number      | <-----------  | - number      |
    | - prev        |               | - prev        |
    +---------------+               +---------------+
    | Level         |               | Level         |
    | - entities    |               | - entities    |
    +---------------+               +---------------+
    | Entity 0      |               | Entity 0      |
    +---------------+               +---------------+
    | Entity 1      |               | Entity 2      |
    +---------------+               +---------------+
    | Image         |               | Image         |
    |               |               |               |
    |               |               +---------------+
    +---------------+               | Entity 3      |
    | Entity 2      |               +---------------+
    +---------------+               .               .
    | Image         |               .               .
    |               |               .               .
    +---------------+               .               .
    | Entity 3      |               .               .
    +---------------+               .               .
    .               .               .               .
    .               .               .               .
    . . . . . . . . .               . . . . . . . . .

There are two big memory arenas.
One for odd frames and one for even frames.
Every frame we read memory from the previous frame arena and write new memory to the current frame arena.
Everything that should persist can has to be copied to the new arena.
Everything else is lost after this frame is finished.
At the end of the frame we clear the previous frame arena and swap the two frame arenas.


### Advantages
- Simple
- Unused memory is instantly freed, basically garbage collection
- I don't have to think about memory freeing

### Disadvantages
- Unsuitable for static data, everything has to be iterated and copied every time.
- In practice, the copying cannot be easily combined with the modifying of the memory.
- Only suitable for small memory footprint
- Pointers are not stable

### Conclusion
This is not good.

While we don't have to think about freeing it actually complicates memory by making me think constantly of coping. Also it is very inefficient.
If your game uses ~10MB of memory and runs at 144 FPS you would need a memory throughput of 1.4 GB/s.

While we can combine all memory updates in this copy. 
However the overhead of copying the data.
Basically garbage collection.

This could also be achieved with Flexible Arenas
