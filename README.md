# Desktop Template

The most minimal, self contained, C99 based, cross platform (Linux, Windows, 
MacOS) desktop application template I could find / put together.

To build from linux, you should be able to just run:

```bash
make build
```

If you have all the libraries installed (see the Makefile for the latest.). Mac
should be very similar. To build on Windows you'll need to setup mingw64.

## Github Actions

The Linux, Windows and Mac version are all auto built from main using Github 
actions. You can find the compiled versions in the artifact section of Github 
Actions. See `build.yml` for more details.

## Header Libraries used and Included

This project has "no dependencies*", the OS windowing / drawing context is done
but the included RGFW library, and audio is done by the Miniaudio library:

- https://github.com/ColleagueRiley/RGFW
- https://miniaud.io/

They can be found in the vendor directory.

*It needs libraries provided by the OS to create windows, and drawing contexts
but you shouldn't need to install them
