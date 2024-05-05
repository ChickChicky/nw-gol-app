# Game Of Life App For Numworks Calculator

Just a small little project that adds a Game Of Life application to a Numworks calculator.

# Quick Start

## First, build the app
This is to ensure that the project builds in the first place, and then to get the binary.
###### *([taken from here](https://github.com/numworks/epsilon-sample-app-c/?tab=readme-ov-file#build-the-app))*

To build this app, you will need to install the [embedded ARM toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) and [Node.js](https://nodejs.org/en/). The C SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) that will automatically be installed at compile time.

```shell
brew install numworks/tap/arm-none-eabi-gcc node # Or equivalent on your OS
make clean && make build
```

You should now have a `output/app.nwa` file that you can distribute! Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

## Then, install it!
###### *([taken from here](https://github.com/numworks/epsilon-sample-app-c/?tab=readme-ov-file#run-the-app-locally))*

```shell
# Now connect your NumWorks calculator to your computer using the USB cable
make run
```

## License

This sample app is distributed under the terms of the BSD License. See LICENSE for details.