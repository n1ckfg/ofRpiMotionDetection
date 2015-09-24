By 
Carl-Johan Rosén
cj.rosen.gmail.com

---------------------------
This is an example on how to use OF without a window, it's tested only on windowsso far.

1. Put ofAppNoWondow.h/cpp in libs/openFrameworks/app and link to your project
2. The example grabs video from your first device and shows in window if window is available
3. You can switch between window and no-window by swapping the use of ofAppNoWindow in main.cpp to using ofAppGlutWindow for example. When NoWindow is used, the draw function is not called. Only update. Mouse and keyboard callbacks are obviously also ignored.
4. Framerate is printed to console.
5. The 'escape' button is the only interface implemented and so far only for windows. Please add + tell me if you add functionality for mac/linux and i will merge! Do this in ofAppNoWindow.cpp, in the update loop.


please give feedback!