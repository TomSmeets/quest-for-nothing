# Creating a Wayland Compositor


## Run a wayland compositor 
To access the eventloop I have to be able to access the insides of the `wl_display`.
To do this just declare it with the first two fields accessible.

```c
struct wl_display {
    struct wl_event_loop *loop;
    int run;
};
```

```c
static void run_event_loop(wl_display *dl) {
    twl->wl_display->run = 1;
    wl_display_flush_clients(twl->wl_display);

    // -1 = block until new events are present
    // 0  = non-blocking
    // n  = block for n ms, or until event
    wl_event_loop_dispatch(twl->wl_display->loop, 0);
}
```

Note that some events can be dispatched during the setup before the `event_loop_dispatch` is called here.
